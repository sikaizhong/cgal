// Copyright (c) 2015 GeometryFactory
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 3 of the License,
// or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// SPDX-License-Identifier: LGPL-3.0+
//
// Author(s)     : Laurent Rineau and Sebastien Loriot

#ifndef CGAL_IO_OFF_READER_H
#define CGAL_IO_OFF_READER_H

#include <CGAL/IO/File_scanner_OFF.h>
#include <CGAL/IO/reader_helpers.h>

#include <vector>
#include <iostream>
#include <CGAL/array.h>
#include <CGAL/assertions.h>
#include <CGAL/use.h>

namespace CGAL {

  template <class Point_3, class Polygon_3>
  bool
  read_OFF( std::istream& in,
            std::vector< Point_3 >& points,
            std::vector< Polygon_3 >& polygons,
            bool /* verbose */ = false)
  {
    CGAL::File_scanner_OFF scanner(in);

    points.resize(scanner.size_of_vertices());
    polygons.resize(scanner.size_of_facets());
    for (std::size_t i = 0; i < scanner.size_of_vertices(); ++i) {
      double x, y, z, w;
      scanner.scan_vertex( x, y, z, w);
      CGAL_assertion(w!=0);
      IO::internal::fill_point( x/w, y/w, z/w, points[i] );
      scanner.skip_to_next_vertex( i);
    }
    if(!in)
      return false;

    for (std::size_t i = 0; i < scanner.size_of_facets(); ++i) {
      std::size_t no;

      scanner.scan_facet( no, i);
      IO::internal::resize(polygons[i], no);
      for(std::size_t j = 0; j < no; ++j) {
        std::size_t id;
        scanner.scan_facet_vertex_index(id, i);
        if(id < scanner.size_of_vertices())
        {
          polygons[i][j] = id;
        }
        else
          return false;
      }
    }
    return in.good();
  }

  template <class Point_3, class Polygon_3, class Color_rgb >
  bool
  read_OFF( std::istream& in,
            std::vector< Point_3 >& points,
            std::vector< Polygon_3 >& polygons,
            std::vector<Color_rgb>& fcolors,
            std::vector<Color_rgb>& vcolors,
            bool /* verbose */ = false)
  {
    CGAL::File_scanner_OFF scanner(in);
    points.resize(scanner.size_of_vertices());
    polygons.resize(scanner.size_of_facets());
    if(scanner.has_colors())
      vcolors.resize(scanner.size_of_vertices());
    bool vcolored = false;
    for (std::size_t i = 0; i < scanner.size_of_vertices(); ++i) {
        double x, y, z, w;
        scanner.scan_vertex( x, y, z, w);
        CGAL_assertion(w!=0);
        IO::internal::fill_point( x/w, y/w, z/w, points[i] );
        if(i == 0)
        {
          std::string col;
          char ci;
          std::getline(in, col);
          std::istringstream iss(col);
          if(iss >> ci){
            std::istringstream iss2(col);
            vcolors[i] = scanner.get_color_from_line(iss2);
            vcolored = true;
          }
        }
        else
        {
          if(vcolored){
            //stores the RGB value
            std::string col;
            std::getline(in, col);
            std::istringstream iss(col);
            vcolors[i] = scanner.get_color_from_line(iss);
          }
        }
        
        if(!in)
          return false;
    }
    bool has_fcolors = false;
    for (std::size_t i = 0; i < scanner.size_of_facets(); ++i) {
      std::size_t no;
      scanner.scan_facet( no, i);
      if(!in)
        return false;
      IO::internal::resize(polygons[i], no);
      for(std::size_t j = 0; j < no; ++j) {
        std::size_t id;
        scanner.scan_facet_vertex_index(id, i);
        if(id < scanner.size_of_vertices())
        {
          polygons[i][j] = id;
        }
        else
        {
          return false;
        }
      }

      if(i==0)
      {
        std::string col;
        std::getline(in, col);
        std::istringstream iss(col);
        char ci =' ';
        if(iss >> ci){
          has_fcolors = true;
          fcolors.resize(scanner.size_of_facets());
          std::istringstream iss2(col);
          fcolors[i] = scanner.get_color_from_line(iss2);
        }
      }
      else if(has_fcolors)
      {
        unsigned char r=0, g=0, b=0;
        scanner.scan_color(r,g,b);
        fcolors[i] = Color_rgb(r,g,b);
      }
    }
    return in.good();
  }

} // namespace CGAL

#endif // CGAL_IO_OFF_READER_H
