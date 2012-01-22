

int function(XMLDocument& doc)
        while (doc.Next()) {
          if (doc.GetName() == "nd") {
            std::string node_id = doc.GetProperty("ref");
            const Point& point = points[node_id];

            float scale = 2000.0/(lat_max-lat_min);
            std::cout << "  Adding point" << std::endl;
            float x = (point.lat - lat_min)*scale;
            float y = (point.lon - lon_min)*scale;
            std::cout << "  Adding point: " << x << ", " << y << std::endl;
            shape.AddPoint(x, y, col, col);
          }
        }
