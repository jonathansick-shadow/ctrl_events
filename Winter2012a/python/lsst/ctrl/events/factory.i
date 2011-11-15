/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
/*
  Implement a more natural wrap for factory methods, for example, if
  you have:

  ----  geometry.h --------
       struct Geometry {                          
         enum GeomType{			     
           POINT,				     
           CIRCLE				     
         };					     
         					     
         virtual ~Geometry() {}    		     
         virtual int draw() = 0;
	 
	 //
	 // Factory method for all the Geometry objects
	 //
         static Geometry *create(GeomType i);     
       };					     
       					     
       struct Point : Geometry  {		     
         int draw() { return 1; }		     
         double width() { return 1.0; }    	     
       };					     
       					     
       struct Circle : Geometry  {		     
         int draw() { return 2; }		     
         double radius() { return 1.5; }          
       }; 					     
       
       //
       // Factory method for all the Geometry objects
       //
       Geometry *Geometry::create(GeomType type) {
         switch (type) {			     
         case POINT: return new Point();	     
         case CIRCLE: return new Circle(); 	     
         default: return 0;			     
         }					     
       }					    
  ----  geometry.h --------


  You can use the %factory with the Geometry::create method as follows:

    %newobject Geometry::create;
    %factory(Geometry *Geometry::create, Point, Circle);
    %include "geometry.h"

  and Geometry::create will return a 'Point' or 'Circle' instance
  instead of the plain 'Geometry' type. For example, in python:

    circle = Geometry.create(Geometry.CIRCLE)
    r = circle.radius()

  where circle is a Circle proxy instance.

  NOTES: remember to fully qualify all the type names and don't
  use %factory inside a namespace declaration, ie, instead of
  
     namespace Foo {
       %factory(Geometry *Geometry::create, Point, Circle);
     }

  use

     %factory(Foo::Geometry *Foo::Geometry::create, Foo::Point,  Foo::Circle);   

     
*/

%define %_factory_dispatch(Type) 
if (!dcast) {
  Type *dobj = dynamic_cast<Type *>($1);
  if (dobj) {
    dcast = 1;
    %set_output(SWIG_NewPointerObj(%as_voidptr(dobj),$descriptor(Type *), $owner | %newpointer_flags));
  }   
}%enddef

%define %factory(Method,Types...)
%typemap(out) Method {
  int dcast = 0;
  %formacro(%_factory_dispatch, Types)
  if (!dcast) {
    %set_output(SWIG_NewPointerObj(%as_voidptr($1),$descriptor, $owner | %newpointer_flags));
  }
}%enddef
