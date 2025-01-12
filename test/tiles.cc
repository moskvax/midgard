#include "test.h"
#include "valhalla/midgard/tiles.h"
#include "valhalla/midgard/aabb2.h"
#include "valhalla/midgard/pointll.h"
#include <iostream>

using namespace std;
using namespace valhalla::midgard;

namespace {

void TestMaxId() {
  if(Tiles<PointLL>::MaxTileId(AABB2<PointLL>(PointLL(-180, -90), PointLL(180, 90)), .25) != 1036799)
    throw std::runtime_error("Unexpected maxid result");
  if(Tiles<PointLL>::MaxTileId(AABB2<PointLL>(PointLL(-180, -90), PointLL(180, 90)), 1) != 64799)
    throw std::runtime_error("Unexpected maxid result");
  if(Tiles<PointLL>::MaxTileId(AABB2<PointLL>(PointLL(-180, -90), PointLL(180, 90)), 4) != 4049)
    throw std::runtime_error("Unexpected maxid result");
  if(Tiles<PointLL>::MaxTileId(AABB2<PointLL>(PointLL(-180, -90), PointLL(180, 90)), .33) != 595685)
    throw std::runtime_error("Unexpected maxid result");
}

void TestBase() {
  Tiles<PointLL> tiles(AABB2<PointLL>(Point2(-180, -90), PointLL(180, 90)), 1);
  PointLL ll;
  // left bottom
  ll = tiles.Base(0);
  if (!(ll.lng() == -180 && ll.lat() == -90)) {
    throw std::runtime_error("Unexpected base result");
  }
  ll = tiles.Base(1);
  if (!(ll.lng() == -179 && ll.lat() == -90)) {
    throw std::runtime_error("Unexpected base result");
  }
  // right bottm
  ll = tiles.Base(179);
  if (!(ll.lng() == 180 && ll.lat() == -90)) {
    throw std::runtime_error("Unexpected base result");
  }
  ll = tiles.Base(180);
  if (!(ll.lng() == -180 && ll.lat() == -89)) {
    throw std::runtime_error("Unexpected base result");
  }
  // right top
  ll = tiles.Base(180 * 180 - 1);
  if (!(ll.lng() == 180 && ll.lat() == 90)) {
    throw std::runtime_error("Unexpected base result");
  }
}

void TestRowCol() {
  Tiles<PointLL> tiles(AABB2<PointLL>(PointLL(-180, -90), PointLL(180, 90)), 1);

  int32_t tileid1 = tiles.TileId(-76.5f, 40.5f);
  auto rc = tiles.GetRowColumn(tileid1);
  int32_t tileid2 = tiles.TileId(rc.second, rc.first);
  if (tileid1 != tileid2) {
    throw std::runtime_error("TileId does not match using row,col");
  }
}

void TestNeighbors() {
  Tiles<PointLL> tiles(AABB2<PointLL>(PointLL(-180, -90), PointLL(180, 90)), 1);

  // Get a tile
  int32_t tileid1 = tiles.TileId(-76.5f, 40.5f);
  auto rc1 = tiles.GetRowColumn(tileid1);

  // Test left neighbor
  int32_t tileid2 = tiles.LeftNeighbor(tileid1);
  auto rc2 = tiles.GetRowColumn(tileid2);
  if (!tiles.AreNeighbors(tileid1, tileid2)) {
    throw std::runtime_error("Left neighbor not identified as a neighbor");
  }
  if (rc1.first != rc2.first || (rc1.second - 1) != rc2.second) {
    throw std::runtime_error("Left neighbor row,col not correct");
  }

  // Test right neighbor
  tileid2 = tiles.RightNeighbor(tileid1);
  rc2 = tiles.GetRowColumn(tileid2);
  if (!tiles.AreNeighbors(tileid1, tileid2)) {
    throw std::runtime_error("Right neighbor not identified as a neighbor");
  }
  if (rc1.first != rc2.first || (rc1.second + 1) != rc2.second) {
    throw std::runtime_error("Right neighbor row,col not correct");
  }

  // Top neighbor
  tileid2 = tiles.TopNeighbor(tileid1);
  rc2 = tiles.GetRowColumn(tileid2);
  if (!tiles.AreNeighbors(tileid1, tileid2)) {
    throw std::runtime_error("Top neighbor not identified as a neighbor");
  }
  if ((rc1.first + 1) != rc2.first || rc1.second != rc2.second) {
    throw std::runtime_error("Top neighbor row,col not correct");
  }

  // Bottom neighbor
  tileid2 = tiles.BottomNeighbor(tileid1);
  rc2 = tiles.GetRowColumn(tileid2);
  if (!tiles.AreNeighbors(tileid1, tileid2)) {
    throw std::runtime_error("Bottom neighbor not identified as a neighbor");
  }
  if ((rc1.first - 1) != rc2.first || rc1.second != rc2.second) {
    throw std::runtime_error("Bottom neighbor row,col not correct");
  }
}

void TileList() {
  Tiles<PointLL> tiles(AABB2<PointLL>(PointLL(-180, -90), PointLL(180, 90)), 1);

  AABB2<PointLL> bbox(PointLL(-99.5f, 30.5f), PointLL(-90.5f, 39.5f));
  std::vector<int32_t> tilelist = tiles.TileList(bbox);
  if (tilelist.size() != 100) {
    throw std::runtime_error("Wrong number of tiles " +
                             std::to_string(tilelist.size()) +
                             " found in TileList");
  }
}

using intersect_t = std::unordered_map<int32_t, std::unordered_set<unsigned short> >;
void assert_answer(const Tiles<Point2>& g, const std::list<Point2>& l, const intersect_t& expected) {
  auto answer = g.Intersect(l);
  /*std::cout << "answer" << std::endl;
  for(auto x : answer) {
    std::cout << std::endl << x.first << ":";
    for(auto y : x.second) {
      std::cout << y << ",";
    }
    std::cout << std::endl;
  }*/
  //wrong number of tiles
  if(answer.size() != expected.size())
    throw std::logic_error("Expected " + std::to_string(expected.size()) + " intersected tiles but got " + std::to_string(answer.size()));
  for(const auto& t : expected) {
    //missing tile
    auto i = answer.find(t.first);
    if(i == answer.cend())
      throw std::logic_error("Expected tile " + std::to_string(t.first) + " to be intersected");
    //wrong number of subdivisions
    if(t.second.size() != i->second.size())
      throw std::logic_error("In tile " + std::to_string(t.first) + " expected " + std::to_string(t.second.size()) + " intersected subdivisions but got " + std::to_string(i->second.size()));
    //missing subdivision
    for(const auto& s : t.second)
      if(i->second.find(s) == i->second.cend())
        throw std::logic_error("In tile " + std::to_string(t.first) + " expected subdivision " + std::to_string(s) + " to be intersected");
  }
}

void test_intersect_linestring() {
  Tiles<Point2> t(AABB2<Point2>{-5,-5,5,5}, 2.5, 5);

  //nothing
  assert_answer(t, {}, intersect_t{});
  assert_answer(t, { {-10,-10} }, intersect_t{});
  assert_answer(t, { {-10,-10}, {-10,-10} }, intersect_t{});

  //single
  assert_answer(t, { {-1,-1} }, intersect_t{{5,{18}}});
  assert_answer(t, { {-1,-1}, {-1,-1} }, intersect_t{{5,{18}}});

  //horizontal
  assert_answer(t, { {-4.9,-4.9}, {4.9,-4.9} }, intersect_t{{0,{0,1,2,3,4}},{1,{0,1,2,3,4}},{2,{0,1,2,3,4}},{3,{0,1,2,3,4}}});
  assert_answer(t, { {-5.9,-4.9}, {5.9,-4.9} }, intersect_t{{0,{0,1,2,3,4}},{1,{0,1,2,3,4}},{2,{0,1,2,3,4}},{3,{0,1,2,3,4}}});
  assert_answer(t, { {-4.9,4.9}, {4.9,4.9} }, intersect_t{{12,{20,21,22,23,24}},{13,{20,21,22,23,24}},{14,{20,21,22,23,24}},{15,{20,21,22,23,24}}});
  assert_answer(t, { {-5.9,4.9}, {5.9,4.9} }, intersect_t{{12,{20,21,22,23,24}},{13,{20,21,22,23,24}},{14,{20,21,22,23,24}},{15,{20,21,22,23,24}}});

  //vertical
  assert_answer(t, { {-4.9,4.9}, {-4.9,-4.9} }, intersect_t{{0,{0,5,10,15,20}},{4,{0,5,10,15,20}},{8,{0,5,10,15,20}},{12,{0,5,10,15,20}}});
  assert_answer(t, { {-4.9,5.9}, {-4.9,-5.9} }, intersect_t{{0,{0,5,10,15,20}},{4,{0,5,10,15,20}},{8,{0,5,10,15,20}},{12,{0,5,10,15,20}}});
  assert_answer(t, { {4.9,4.9}, {4.9,-4.9} }, intersect_t{{3,{4,9,14,19,24}},{7,{4,9,14,19,24}},{11,{4,9,14,19,24}},{15,{4,9,14,19,24}}});
  assert_answer(t, { {4.9,5.9}, {4.9,-5.9} }, intersect_t{{3,{4,9,14,19,24}},{7,{4,9,14,19,24}},{11,{4,9,14,19,24}},{15,{4,9,14,19,24}}});

  //diagonal
  assert_answer(t, { {-4.9,-4.9}, {4.9,4.9} }, intersect_t{{0,{0,6,12,18,24}},{5,{0,6,12,18,24}},{10,{0,6,12,18,24}},{15,{0,6,12,18,24}}});
  assert_answer(t, { {-5.9,-5.9}, {5.9,5.9} }, intersect_t{{0,{0,6,12,18,24}},{5,{0,6,12,18,24}},{10,{0,6,12,18,24}},{15,{0,6,12,18,24}}});
  assert_answer(t, { {-4.9,4.9}, {4.9,-4.9} }, intersect_t{{12,{20,16,12,8,4}},{9,{20,16,12,8,4}},{6,{20,16,12,8,4}},{3,{20,16,12,8,4}}});
  assert_answer(t, { {-5.9,5.9}, {5.9,-5.9} }, intersect_t{{12,{20,16,12,8,4}},{9,{20,16,12,8,4}},{6,{20,16,12,8,4}},{3,{20,16,12,8,4}}});

  //random slopes
  t = Tiles<Point2>(AABB2<Point2>{0,0,6,6}, 6, 6);
  assert_answer(t, { {0.5,0.5}, {5.5,4.5} }, intersect_t{{0,{0,1,7,8,14,15,21,22,28,29}}});
  assert_answer(t, { {5.5,4.5}, {0.5,0.5} }, intersect_t{{0,{0,1,7,8,14,15,21,22,28,29}}});
  assert_answer(t, { {5.5,0.5}, {0.5,2.5} }, intersect_t{{0,{4,5,7,8,9,10,12,13}}});
  assert_answer(t, { {0.5,2.5}, {5.5,0.5} }, intersect_t{{0,{4,5,7,8,9,10,12,13}}});
  assert_answer(t, { {-1,-2}, {4,8} }, intersect_t{{0,{0,6,13,19,26,32}}});
  assert_answer(t, { {4,8}, {-1,-2} }, intersect_t{{0,{0,6,13,19,26,32}}});
  assert_answer(t, { {1,2}, {2,4} }, intersect_t{{0,{13,19,26}}});
  assert_answer(t, { {2,4}, {1,2} }, intersect_t{{0,{13,19,26}}});
}
/*
void test_intersect_circle() {
  grid<Point2> g(AABB2<Point2>{-1,-1,1,1}, 5);
  //TODO:
}

void test_random_linestring() {
  grid<Point2> g(AABB2<Point2>{-1,-1,1,1}, 5);
  std::default_random_engine generator;
  std::uniform_real_distribution<> distribution(-10, 10);
  for(int i = 0; i < 10000; ++i) {
    std::vector<Point2> linestring;
    for(int j = 0; j < 100; ++j)
      linestring.emplace_back(PointLL(distribution(generator), distribution(generator)));
    bool leaves;
    auto answer = g.intersect(linestring, leaves);
    for(auto a : answer)
      if(a > 24)
        throw std::runtime_error("Non-existant cell!");
  }
}

void test_random_circle() {
  grid<Point2> g(AABB2<Point2>{-1,-1,1,1}, 5);
  //TODO:
}
*/
}

int main() {
  test::suite suite("tiles");

  // Test tile id to row, col and vice-versa
  suite.test(TEST_CASE(TestRowCol));

  // Test neighbors
  suite.test(TEST_CASE(TestNeighbors));

  // Test max. tile Id
  suite.test(TEST_CASE(TestMaxId));

  // Test tile list
  suite.test(TEST_CASE(TileList));

  suite.test(TEST_CASE(test_intersect_linestring));
  /*suite.test(TEST_CASE(test_intersect_circle));
  suite.test(TEST_CASE(test_random_linestring));
  suite.test(TEST_CASE(test_random_circle));*/

  return suite.tear_down();
}
