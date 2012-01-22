
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include "XMLDocument.h"
#include "polypartition.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>

class Point {
  public:
    Point(float la=0, float lo=0) : lat(la), lon(lo) {};
    float lat;
    float lon;
};

class Polygon {
  public:
    Polygon() {};
    void add_node(const Point& node) { nodes_.push_back(node);};
    std::vector<Point> nodes_;
};
class Shape {
  public:
    Shape() {};
    void add_polygon(const Polygon& polygon = Polygon()) { polygons_.push_back(polygon);}
    void add_node(const Point& node) {
      if (polygons_.size() == 0) add_polygon();
      polygons_[polygons_.size()-1].add_node(node);
    }

    void partition() {
      TPPLPoly poly;
      std::vector<Point> nodes = polygons_[0].nodes_;
      poly.Init(nodes.size());
      unsigned int i = 0;
      for (std::vector<Point>::const_iterator p = nodes.begin(); p != nodes.end(); ++p, ++i) {
        poly[i].x = p->lat;
        poly[i].y = p->lon;
      }
      std::list<TPPLPoly> convex_polys;
      TPPLPartition partitioner;
      partitioner.Triangulate_OPT(&poly, &convex_polys);
      //partitioner.ConvexPartition_HM(&poly, &convex_polys);
      polygons_.clear();
      for (std::list<TPPLPoly>::iterator p = convex_polys.begin(); p != convex_polys.end(); ++p) {
        add_polygon();
        for (long i = 0; i < p->GetNumPoints(); ++i) {
          TPPLPoint point = p->GetPoint(i);
          add_node(Point(point.x, point.y));
        }
      }
    }

    std::vector<Polygon> polygons_;
};

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
  std::vector<sf::Shape> shapes;
  std::map<std::string, Point> points;
  float lat_min, lat_max, lon_min, lon_max;
  XMLDocument doc("map.osm");
  if (doc.GetName() == "osm") {
    doc.Enter();
    while (doc.Next()) {
      if (doc.GetName() == "bounds") {
        std::string minlat = doc.GetProperty("minlat");
        lat_min = atof(minlat.c_str());
        std::string minlon = doc.GetProperty("minlon");
        lon_min = atof(minlon.c_str());
        std::string maxlat = doc.GetProperty("maxlat");
        lat_max = atof(maxlat.c_str());
        std::string maxlon = doc.GetProperty("maxlon");
        lon_max = atof(maxlon.c_str());
      } else if (doc.GetName() == "node") {
        std::string lat = doc.GetProperty("lat");
        float lattitude = atof(lat.c_str());
        std::string lon = doc.GetProperty("lon");
        float longitude = atof(lon.c_str());
        std::string id(doc.GetProperty("id"));
        points[id] = Point(lattitude, longitude);
      } else if (doc.GetName() == "way") {
        std::cout << "Adding shape" << std::endl;
        std::string type = "";
        doc.Enter();
        while (doc.Next()) {
          if (doc.GetName() == "tag") {
            if (doc.GetProperty("k") == "building") {
              if (doc.GetProperty("v") == "yes") {
                type = "building";
                break;
              }
            } else if (doc.GetProperty("k") == "natural") {
              if (doc.GetProperty("v") == "water") {
                type = "water";
                break;
              }
            } else if (doc.GetProperty("k") == "landuse") {
              if (doc.GetProperty("v") == "grass") {
                type = "grass";
                break;
              } else if (doc.GetProperty("v") == "residential") {
                type = "residential";
                break;
              }
            }
          }
        }
        doc.First();
        sf::Shape shape;
        shape.EnableFill(false);
        shape.EnableOutline(false);
        //shape.SetOutlineWidth(2);
        sf::Color col(255, 255, 255);
        if (type == "building") {
          std::cout << "Type = building" << std::endl;
          col = sf::Color(100, 100, 100, 50);
          shape.EnableFill(true);
        } else if (type == "water") {
          std::cout << "Type = water" << std::endl;
          col = sf::Color(100, 100, 255, 50);
          shape.EnableFill(true);
        } else if (type == "grass") {
          std::cout << "Type = grass" << std::endl;
          col = sf::Color(100, 255, 100, 50);
          shape.EnableFill(true);
        } else if (type == "residential") {
          std::cout << "Type = grass" << std::endl;
          col = sf::Color(200, 200, 200, 50);
          shape.EnableFill(true);
        }
        Shape shp;
        while (doc.Next()) {
          if (doc.GetName() == "nd") {
            std::string node_id = doc.GetProperty("ref");
            const Point& point = points[node_id];
            shp.add_node(point);
          }
        }
        shp.partition();

        for (std::vector<Polygon>::const_iterator p = shp.polygons_.begin(); p != shp.polygons_.end(); ++p) {
          sf::Shape new_shape = shape;
          for (std::vector<Point>::const_iterator q = p->nodes_.begin(); q != p->nodes_.end(); ++q) {
            float scale = 2000.0/(lat_max-lat_min);
            float x = (q->lat - lat_min)*scale;
            float y = (q->lon - lon_min)*scale;
            new_shape.AddPoint(x, y, col, col);
          }
          shapes.push_back(new_shape);
        }

        std::cout << "  pushing shape" << std::endl;
        shapes.push_back(shape);
        doc.Leave();
      }
    }
  }
  float scale = 2000.0/(lat_max-lat_min);
    
    // Create the main window
    sf::RenderWindow App(sf::VideoMode(800, 600), "SFML Views");

    // Create a view with the same size as the window, located somewhere in the center of the background
    sf::Vector2f Center(1000, 1000);
    sf::Vector2f HalfSize(400, 300);
    sf::View View(Center, HalfSize);

    // Start game loop
    while (App.IsOpened())
    {
        // Process events
        sf::Event Event;
        while (App.GetEvent(Event))
        {
            // Close window : exit
            if (Event.Type == sf::Event::Closed)
                App.Close();

            // Escape key : exit
            if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
                App.Close();
        }

        // Move the view using arrow keys
        float Offset = 200.f * App.GetFrameTime();
        if (App.GetInput().IsKeyDown(sf::Key::Up))    View.Move( 0,      -Offset);
        if (App.GetInput().IsKeyDown(sf::Key::Down))  View.Move( 0,       Offset);
        if (App.GetInput().IsKeyDown(sf::Key::Left))  View.Move(-Offset,  0);
        if (App.GetInput().IsKeyDown(sf::Key::Right)) View.Move( Offset,  0);

        // Zoom and unzoom using + and - keys
        if (App.GetInput().IsKeyDown(sf::Key::Add))      View.Zoom(1.001f);
        if (App.GetInput().IsKeyDown(sf::Key::Subtract)) View.Zoom(0.999f);

        if (App.GetInput().IsKeyDown(sf::Key::A))      View.Zoom(1.011f);
        if (App.GetInput().IsKeyDown(sf::Key::Z)) View.Zoom(0.989f);

        // Set the view
        App.SetView(View);

        // Clear screen
        App.Clear();

        for (std::vector<sf::Shape>::iterator p = shapes.begin(); p != shapes.end(); ++p) {
          App.Draw(*p);
        }

        // Finally, display rendered frame on screen
        App.Display();
    }

    return EXIT_SUCCESS;
}

