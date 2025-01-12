#ifndef VALHALLA_MIDGARD_AABB2_H_
#define VALHALLA_MIDGARD_AABB2_H_

#include <vector>
#include <valhalla/midgard/linesegment2.h>

namespace valhalla {
namespace midgard {

/**
 * Axis Aligned Bounding Box (2 dimensional). This is a template class that
 * works with Point2 (Euclidean x,y) or PointLL (latitude,longitude).
 *
 *
 * TODO: merge clipper2 class into this one, its basically a thin scaffold around
 * this class anyway and has some useful intersection stuff
 */
template <class coord_t>
class AABB2 {
 public:
  using x_t = typename coord_t::first_type;
  using y_t = typename coord_t::second_type;

  /**
   * Default constructor.
   */
  AABB2();

  /**
   * Construct an AABB given a minimum and maximum point.
   * @param  minpt  Minimum point (x,y)
   * @param  maxpt  Maximum point (x,y)
   */
  AABB2(const coord_t& minpt, const coord_t& maxpt);

  /**
   * Constructor with specified bounds.
   * @param   minx    Minimum x of the bounding box.
   * @param   miny    Minimum y of the bounding box.
   * @param   maxx    Maximum x of the bounding box.
   * @param   maxy    Maximum y of the bounding box.
   */
  AABB2(const x_t minx, const y_t miny,
        const x_t maxx, const y_t maxy);

  /**
   * Construct an AABB given a list of points.
   * @param  pts  Vertex list.
   */
  AABB2(const std::vector<coord_t>& pts);

  /**
   * Equality operator.
   * @param   r2  Bounding box to compare to.
   * @return  Returns true if the 2 bounding boxes are equal.
   */
  bool operator ==(const AABB2& r2) const;

  /**
   * Get the minimum x
   * @return  Returns minimum x.
   */
  x_t minx() const;

  /**
   * Get the maximum x
   * @return  Returns maximum x.
   */
  x_t maxx() const;

  /**
   * Get the minimum y
   * @return  Returns minimum y.
   */
  y_t miny() const;

  /**
   * Get the maximum y
   * @return  Returns maximum y.
   */
  y_t maxy() const;

  /**
   * Get the point at the minimum x,y.
   * @return  Returns the min. point.
   */
  coord_t minpt() const;

  /**
   * Get the point at the maximum x,y.
   * @return  Returns the max. point.
   */
  coord_t maxpt() const;

  /**
   * Creates an AABB given a list of points.
   * @param  pts  Vertex list.
   */
  void Create(const std::vector<coord_t>& pts);

  /**
   * Gets the center of the bounding box.
   * @return  Returns the center point of the bounding box.
   */
  coord_t Center() const;

  /**
   * Tests if a specified point is within the bounding box.
   * @param   pt   Point to test.
   * @return  Returns true if within the bounding box and false if outside
   *          the extent. Points that lie along the minimum x or y edge are
   *          considered to be inside, while points that lie on the maximum
   *          x or y edge are considered to be outside.
   */
  bool Contains(const coord_t& pt) const;

  /**
   * Checks to determine if another bounding box is completely inside this
   * bounding box.
   * @param   r2    Test bounding box
   * @return  Returns false if the bounding box is not entirely inside,
   *          true if it is inside.
   */
  bool Contains(const AABB2& r2) const;

  /**
   * Test if this bounding box intersects another bounding box.
   * @param    r2   Other bounding box to test intersection against.
   * @return   Returns true if the bounding box intersect, false if not.
   */
  bool Intersects(const AABB2& r2) const;

  /**
   * Tests whether the segment intersects the bounding box.
   * @param   seg  Line segment
   * @return  Returns true if the segment intersects (or lies completely
   *          within) the bounding box.
   */
  bool Intersects(const LineSegment2<coord_t>& seg) const;

  /**
   * Tests whether the segment intersects the bounding box.
   * @param   a  Endpoint of the segment
   * @param   b  Endpoint of the segment
   * @return  Returns true if the segment intersects (or lies completely
   *          within) the bounding box.
   */
  bool Intersects(const coord_t& a, const coord_t& b) const;

  /**
   * Tests whether the circle intersects the bounding box.
   * @param   center  center of circle
   * @param   radius  radius of the circle
   * @return  Returns true if the circle intersects (or lies completely
   *          within) the bounding box.
   */
  bool Intersects(const coord_t& center, const float radius) const;

  /**
   * Clips the input set of vertices to the specified boundary.  Uses a
   * method where the shape is clipped against each edge in succession.
   * @param    pts      In/Out. List of points in the polyline/polygon.
   *                    After clipping this list is clipped to the boundary.
   * @param    closed   Is the shape closed?
   * @return   Returns the number of vertices in the clipped shape. May have
   *           0 vertices if none of the input polyline intersects or lies
   *           within the boundary.
   */
  uint32_t Clip(std::vector<coord_t>& pts, const bool closed) const;

  /**
   * Intersects the segment formed by u,v with the bounding box
   *
   * @param  u  the first point in the segment
   * @param  v  the second point in the segment
   * @return Returns true if the segment actually intersected the bounding box
   *         and moves u and v to actually reflect the intersection points
   */
  bool Intersect(coord_t& u, coord_t& v) const;

  /**
   * Gets the width of the bounding box.
   * @return  Returns the width of this bounding box.
   */
  x_t Width() const;

  /**
   * Gets the height of the bounding box.
   * @return  Returns the height of this bounding box.
   */
  y_t Height() const;

  /**
   * Expands (if necessary) the bounding box to include the specified
   * bounding box.
   * @param  r2  Bounding bounding box to "combine" with this bounding box.
   */
  void Expand(const AABB2& r2);

 protected:
  // Edge to clip against
  enum ClipEdge { kLeft, kRight, kBottom, kTop };

  // Minimum and maximum x,y values (lower left and upper right corners)
  // of a rectangle / bounding box.
  x_t minx_;
  y_t miny_;
  x_t maxx_;
  y_t maxy_;

  /**
   * Clips the polyline/polygon against a single edge.
   * @param  edge    Edge to clip against.
   * @param  closed  True if the vertices form a polygon.
   * @param  vin     List of input vertices.
   * @param  vout    Output vertices.
   * @return  Returns the number of vertices after clipping. The list
   *          of vertices is in vout.
   */
  uint32_t ClipAgainstEdge(const ClipEdge edge, const bool closed,
            const std::vector<coord_t>& vin, std::vector<coord_t>& vout) const;

  /**
   * Finds the intersection of the segment from insidept to outsidept with the
   * specified boundary edge.  Finds the intersection using the parametric
   * line equation.
   * @param  edge  Edge to intersect.
   * @param  insidept  Vertex inside with respect to the edge.
   * @param  outsidept Vertex outside with respect to the edge.
   * @return  Returns the intersection of the segment with the edge.
   */
  coord_t ClipIntersection(const ClipEdge edge, const coord_t& insidept,
                          const coord_t& outsidept) const;

  /**
   * Tests if the vertex is inside the rectangular boundary with respect to
   * the specified edge.
   * @param   edge   Edge to test.
   * @param   v      Vertex to test.
   * @return  Returns true if the point is inside with respect to the edge,
   *          false if it is outside.
   */
  bool Inside(const ClipEdge edge, const coord_t& v) const;

  /**
   * Adds a vertex to the output vector if not equal to the prior.
   * @param  pt    Vertex to add.
   * @param  vout  Vertex list.
   */
  void Add(const coord_t& pt, std::vector<coord_t>& vout) const;
};

}
}

#endif  // VALHALLA_MIDGARD_AABB2_H_
