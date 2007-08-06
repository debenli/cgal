#ifndef CGAL_AOS3_CROSS_SECTION_QT_VIEWER_MARKUP_H
#define CGAL_AOS3_CROSS_SECTION_QT_VIEWER_MARKUP_H
#include <CGAL/Arrangement_of_spheres_3_basic.h>
#include <CGAL/IO/Qt_examiner_viewer_2.h>
#include <CGAL/Arrangement_of_spheres_3/Rational_cross_section.h>
#include <CGAL/Simple_cartesian.h>
CGAL_AOS3_BEGIN_INTERNAL_NAMESPACE

CGAL_AOS3_TEMPLATE
class Cross_section_qt_viewer_markup {
  CGAL_AOS3_TRAITS;
  typedef Combinatorial_cross_section CGAL_AOS3_TARG CS;
  typedef Rational_cross_section CGAL_AOS3_TARG RCS;
  typedef CGAL::Simple_cartesian<double> K;
public:
  typedef CGAL_AOS3_TYPENAME Traits::FT NT;
  Cross_section_qt_viewer_markup(Traits tr,  const CS &cs,
				 CGAL::Layer l,
				 CGAL::Color col= CGAL::RED): tr_(tr),
							      cs_(cs),
							      rcs_(cs, tr_),
							      layer_(l), 
							      color_(col)
  {}

  void operator()(NT z, Qt_examiner_viewer_2 *qtv) {
    rcs_.set_z(z);
    *qtv << layer_;
    *qtv << color_;
    qtv->set_updating_box(false);
    for (CGAL_AOS3_TYPENAME CS::Halfedge_const_iterator hit= cs_.halfedges_begin();
	 hit != cs_.halfedges_end(); ++hit){
      bool marked= (faces_.find(hit->face()) != faces_.end() 
		    || faces_.find(hit->opposite()->face()) != faces_.end())
	|| (edges_.find(hit) != edges_.end()
	    || edges_.find(hit->opposite()) != edges_.end());
      if (!marked) continue;
      qtv->set_updating_box(false);
      if (hit->curve().is_rule() && hit->curve().is_inside()){
	
	//std::cout << "Displaying rule " << hit->curve() << std::endl;
	CGAL_AOS3_TYPENAME K::Point_2 t= display_point(hit->vertex()->point());
	CGAL_AOS3_TYPENAME K::Point_2 s= display_point(hit->opposite()->vertex()->point());
	
	*qtv << CGAL::RED;
     
	*qtv << CGAL_AOS3_TYPENAME K::Segment_2(t,s);
      } else if (hit->curve().is_arc() && hit->curve().is_inside()){
	qtv->set_updating_box(true);
	//std::cout << "Displaying arc " << hit->curve() << std::endl;
	CGAL_AOS3_TYPENAME K::Point_2 t= display_point(hit->vertex()->point());
	CGAL_AOS3_TYPENAME K::Point_2 s= display_point(hit->opposite()->vertex()->point());
	//DT::Circle_2 c= ;
	
	*qtv << CGAL::RED;
      
	qtv->new_circular_arc(rcs_.circle(hit->curve().key()), s, t);
      }
    }
 
   
    for (CGAL_AOS3_TYPENAME CS::Vertex_const_iterator hit= cs_.vertices_begin();
	 hit != cs_.vertices_end(); ++hit){
      bool marked= (vertices_.find(hit) != vertices_.end());
      if (!marked) continue;
      
      CGAL_AOS3_TYPENAME K::Point_2 p= display_point(hit->point());
      
      *qtv << CGAL::RED;
      
      if (hit->point().is_finite()) {
	qtv->set_updating_box(true);
      } else {
	qtv->set_updating_box(false);
      }
      *qtv << p;
      
      std::ostringstream out;
      CGAL_AOS3_TYPENAME CS::Point pt= hit->point();
      
      if (pt.is_sphere_extremum()) {
	out << pt.sphere_key();
      } else if (pt.is_sphere_rule()) {
	out << pt.sphere_key() << ":" << pt.rule_key();
      } else if (pt.is_rule_rule()) {
	out << pt.rule_key(plane_coordinate(0)) << ":" << pt.rule_key(plane_coordinate(1));
      } else {
	out << pt.sphere_key(0) << ":" << pt.sphere_key(1);
      }
      
      *qtv << CGAL::GRAY;
      *qtv << out.str().c_str();
      qtv->set_is_dirty(true);
    }
  }

  void clear() {
    faces_.clear();
    edges_.clear();
    vertices_.clear();
  }

  void new_face(CGAL_AOS3_TYPENAME CS::Face_const_handle f){
    faces_.insert(f);
  }
  void new_edge(CGAL_AOS3_TYPENAME CS::Halfedge_const_handle f){
    edges_.insert(f);
  }
  void new_vertex(CGAL_AOS3_TYPENAME CS::Vertex_const_handle f){
    vertices_.insert(f);
  }
  
private:

  CGAL_AOS3_TYPENAME K::Point_2 display_point(CGAL_AOS3_TYPENAME CS::Point pt) const {
    try {
      CGAL_AOS3_TYPENAME Traits::Sphere_point_3 sp= rcs_.sphere_point(pt);
      //std::cout << "Exact point is " << sp << std::endl;
      return CGAL_AOS3_TYPENAME K::Point_2(sp.approximate_coordinate(plane_coordinate(0)),
					   sp.approximate_coordinate(plane_coordinate(1)));
    } catch (CGAL_AOS3_TYPENAME Traits::Point_3 pt) {
      std::cout << "Point " << pt << " is no longer valid at " << rcs_.z() << std::endl;
      return CGAL_AOS3_TYPENAME K::Point_2(CGAL::to_double(pt[plane_coordinate(0).index()]),
					   CGAL::to_double(pt[plane_coordinate(1).index()]));
    }
  }
  struct Handle_compare{
    template <class H>
    bool operator()(H a, H b) const {
      return &*a < &*b;
    }
  };

  std::set<CGAL_AOS3_TYPENAME CS::Face_const_handle, Handle_compare> faces_;
  std::set<CGAL_AOS3_TYPENAME CS::Halfedge_const_handle, Handle_compare> edges_;
  std::set<CGAL_AOS3_TYPENAME CS::Vertex_const_handle, Handle_compare> vertices_;

  Traits tr_;
  const CS &cs_;
  RCS rcs_;
  Layer layer_;
  CGAL::Color color_;
};

CGAL_AOS3_END_INTERNAL_NAMESPACE


#endif
