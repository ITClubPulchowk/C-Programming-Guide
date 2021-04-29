#include <SDL.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#define TOP_DOWN
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int SITE_RECT_H = 20;
const int SITE_RECT_W = 20;
const int BOUNDING_BOX_XMIN = 50;
const int BOUNDING_BOX_YMIN = 50;
const int BOUNDING_BOX_XMAX = 700;
const int BOUNDING_BOX_YMAX = 700;
const int INITIAL_SITE_X = 150;
const int INITIAL_SITE_Y = 400;

#define EPSILON 0.00000001f

typedef struct v2 {
  float x, y;
} v2;

typedef struct Line {
  v2 p, v;
} Line;

typedef struct LineSegment {
  v2 start, end;
} LineSegment;

typedef struct Edge {
  int vindex;
  struct Region *region;
  struct Edge *twin;
  struct Edge *next;
  struct Edge *prev;
} Edge;

typedef struct Region {
  struct Edge *incident_edge;
  v2 site;
} Region;

typedef struct Vertex {
  v2 point;
  struct Edge *inc_edge;
} Vertex;

typedef struct Vonoroi {
  v2 *sites;
  Vertex *vertices;
  LineSegment *debug_lines;
  Region **region_list;
  Edge **common_edges;
  Region **neighboring_regions;

  v2 *point_render_buffer;
} Vonoroi;

void *xmalloc(size_t n) {
  void *p = malloc(n);
  if (!p) {
    fprintf(stderr, "malloc failed! Exiting...\n");
    exit(1);
  }
  return p;
}

void *xcalloc(size_t n, size_t size) {
  void *p = calloc(n, size);
  if (!p) {
    fprintf(stderr, "calloc failed! Exiting...\n");
    exit(1);
  }
  return p;
}

Vertex create_vertex(v2 p);
Edge *create_edge(int vidnex);

int draw_line_segment(SDL_Renderer *r, LineSegment *l);

v2 line_point(const Line *l, float t);
int line_line_segment_intersect(const Line *line, const LineSegment *segment,
                                v2 *ret);
Line perpendicular_bisector(v2 start, v2 end);
int half_line_line_segment_intersect(const Line *half_line,
                                     const LineSegment *segment, v2 *ret);

void print_region_vertices(Region *r, Vertex *v);
int inside_region(Edge *edge, v2 z, Vertex *v);
int find_vonoroi_region(Vonoroi *v, v2 point);
void split_edges(Vonoroi *v, Edge *e, v2 point);
Region *split_region(Vonoroi *v, Region *r, v2 new_site, Edge *edge_list[]);
void merge_regions(Vonoroi *v, Region *r1, Region *r2);
Region **internal_delete_region(Region **list, Region *r);
// Required for stb.
#define delete_region(x1, x2) ((x1) = internal_delete_region((x1), (x2)))

void render_vonoroi(SDL_Renderer *r, Vonoroi *v);
void destroy_vonoroi(Vonoroi *v);

v2 create_point(float x, float y) { return (v2){x, y}; }

Edge *create_edge(int vidnex) {
  Edge *e = xcalloc(1, sizeof(*e));
  e->vindex = vidnex;
  return e;
}

Vertex create_vertex(v2 p) { return (Vertex){.point = p}; }

/* Returns 1 if the half-line (t>=0) half_line and the line segment segment
 * intersect each other, 0 otherwise. If ret is not NULL, the intersection point
 * is stored in ret.
 */
int half_line_line_segment_intersect(const Line *half_line,
                                     const LineSegment *segment, v2 *ret) {
  float x1 = segment->start.x, y1 = segment->start.y;
  float x2 = segment->end.x, y2 = segment->end.y;

  float x3 = half_line->p.x, y3 = half_line->p.y;

  v2 p = line_point(half_line, 1.0f);
  float x4 = p.x, y4 = p.y;

  float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

  if (fabs(denom) < EPSILON) {
    return 0;
  }

  float n1 = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
  float t1 = n1 / denom;
  if (t1 < 0 || t1 > 1.0f) {
    return 0;
  }
  float n2 = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
  float t2 = n2 / denom;

  if (t2 < 0) {
    return 0;
  }

  if (ret) {
    *ret = (v2){.x = x1 + (x2 - x1) * t1, .y = y1 + (y2 - y1) * t1};
  }
  return 1;
}
/* Returns 1 if the line line and the line segment segment intersect each other,
 * 0 otherwise.
 * If ret is not NULL, the intersection point is stored in ret.
 */

int line_line_segment_intersect(const Line *line, const LineSegment *segment,
                                v2 *ret) {
  float x1 = segment->start.x, y1 = segment->start.y;
  float x2 = segment->end.x, y2 = segment->end.y;

  float x3 = line->p.x, y3 = line->p.y;
  float x4 = line->p.x + line->v.x, y4 = line->p.y + line->v.y;

  float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

  if (fabs(denom) < EPSILON) {
    return 0;
  }

  float n = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
  float t = n / denom;

  if (t < 0 || t > 1.0f) {
    return 0;
  }

  if (ret) {
    *ret = (v2){.x = x1 + (x2 - x1) * t, .y = y1 + (y2 - y1) * t};
  }
  return 1;
}

/* Returns the perpendicular bisector of the point start and end
 */
Line perpendicular_bisector(v2 start, v2 end) {
  v2 mid = (v2){0.5f * (start.x + end.x), 0.5f * (start.y + end.y)};
  v2 dir = {.x = (end.y - start.y), .y = -(end.x - start.x)};

  return (Line){.p = mid, .v = dir};
}

/* Returns a point on the line l given by parameter t */
v2 line_point(const Line *l, float t) {
  return (v2){l->p.x + t * l->v.x, l->p.y + t * l->v.y};
}

void print_region_vertices(Region *r, Vertex *v) {
  Edge *iter = r->incident_edge;
  do {
    v2 p = v[iter->vindex].point;
    printf("%f, %f\n", p.x, p.y);
    iter = iter->next;
  } while (iter != r->incident_edge);
}

/* Returns 1 if the point z lies within polygon whose one edge is
 * given by edge, 0 otherwise */
int inside_region(Edge *edge, v2 z, Vertex *v) {
  Line horizontal = {.p = z, .v = {1, 0}};
  int n = 0;

  Edge *iter = edge;
  Edge *intersections[2];
  int len = 0;
  do {
    v2 p1 = v[iter->vindex].point;
    v2 p2 = v[iter->twin->vindex].point;
    LineSegment segment = {.start = p1, .end = p2};
    if (half_line_line_segment_intersect(&horizontal, &segment, NULL)) {
      intersections[len++] = iter;
      assert(len <= 2);
      n += 1;
    }
    iter = iter->next;
  } while (iter != edge);

  if (len == 1 || len == 0)
    return len;
  assert(len == 0 || len == 2);
  int e1start = intersections[0]->vindex,
      e1end = intersections[0]->twin->vindex;
  int e2start = intersections[1]->vindex,
      e2end = intersections[1]->twin->vindex;
  // Maybe compare with actual values instead of indices.
  if (e1start == e2end) {
#ifdef TOP_DOWN
    n = 0;
    n += (v[e1end].point.y > z.y);
    n += (v[e2start].point.y > z.y);
#else
#error "Co-ordinate system is not top down!"
#endif
  } else if (e1end == e2start) {
#ifdef TOP_DOWN
    n = 0;
    n += (v[e1start].point.y > z.y);
    n += (v[e2end].point.y > z.y);
#else
#error "Co-ordinate system is not top down!"
#endif
  }
  return n & 1;
}

/* Retuns the index of the region in which the point lies in*/
/* -1 if the point does not lie within the region. */
int find_vonoroi_region(Vonoroi *v, v2 point) {
  for (int i = 0; i < arrlenu(v->region_list); i++) {
    Region *r = v->region_list[i];
    Edge *incident_edge = r->incident_edge;
    if (inside_region(incident_edge, point, v->vertices)) {
      return i;
    }
  }
  return -1;
}

/* Split the edge given by e into two edges at the given point */
void split_edges(Vonoroi *v, Edge *e, v2 point) {
  Vertex vertex = create_vertex(point);
  arrpush(v->vertices, vertex);
  int vindex = arrlen(v->vertices) - 1;

  Edge *new = create_edge(vindex);
  new->region = e->region;
  new->twin = e->twin;
  new->next = e->next;
  new->prev = e;

  Edge *twin = create_edge(vindex);
  twin->region = e->twin->region;
  twin->twin = e;
  twin->next = e->twin->next;
  twin->prev = e->twin;

  e->next->prev = new;
  e->next = new;

  e->twin->next->prev = twin;
  e->twin->next = twin;

  e->twin->twin = new;
  e->twin = twin;
}

// Returns the region index of the newly created region which contains the new
// site Split the given region into two separate regions. The array edge_list[]
// contains two edges which split the region
Region *split_region(Vonoroi *v, Region *r, v2 new_site, Edge *edge_list[]) {
  int v1index = edge_list[0]->vindex;
  int v2index = edge_list[1]->vindex;

  Edge *e = create_edge(v1index);
  Edge *e_twin = create_edge(v2index);

  e->twin = e_twin;
  e_twin->twin = e;

  e_twin->next = edge_list[0];
  e->prev = edge_list[0]->prev;

  edge_list[0]->prev->next = e;
  e_twin->next->prev = e_twin;

  e->next = edge_list[1];

  e_twin->prev = edge_list[1]->prev;
  edge_list[1]->prev->next = e_twin;

  edge_list[1]->prev = e;

  Region *old_region = r;
  Region *new_region = xmalloc(sizeof(Region));
  new_region->site = new_site;

  Region *e_region, *e_twin_region;

  if (inside_region(e, old_region->site, v->vertices)) {
    old_region->incident_edge = e;
    new_region->incident_edge = e_twin;

    e_region = old_region;
    e_twin_region = new_region;
  } else {
    old_region->incident_edge = e_twin;
    new_region->incident_edge = e;

    e_region = new_region;
    e_twin_region = old_region;
  }

  Edge *iter = e_twin;
  do {
    iter->region = e_twin_region;
    iter = iter->next;
  } while (iter != e_twin);

  iter = e;
  do {
    iter->region = e_region;
    iter = iter->next;
  } while (iter != e);

  arrpush(v->region_list, new_region);
  return new_region;
}

/* Merge the two regions r1 and r2 into a single region.
 * r2 is deleted along with all other common edges of the regions.
 */
void merge_regions(Vonoroi *v, Region *r1, Region *r2) {
  assert(r1);
  assert(r2);

  arrsetlen(v->common_edges, 0);

  Edge *iter1 = r1->incident_edge;
  // Find one edge which is not common
  // There is prob. better way to do this, but whatever.
  bool is_iter1_common = false;
  do {
    is_iter1_common = false;
    Edge *iter2 = r2->incident_edge;
    do {
      if (iter1->vindex == iter2->twin->vindex &&
          iter2->vindex == iter1->twin->vindex) {
        is_iter1_common = true;
      }
      iter2 = iter2->next;
    } while (iter2 != r2->incident_edge);
    iter1 = iter1->next;
  } while (iter1 != r1->incident_edge && is_iter1_common);
  Edge *temp = iter1;
  // Find the edges which are common
  do {
    Edge *iter2 = r2->incident_edge;
    do {
      if (iter1->vindex == iter2->twin->vindex &&
          iter2->vindex == iter1->twin->vindex) {
        arrpush(v->common_edges, iter1);
      }
      iter2 = iter2->next;
    } while (iter2 != r2->incident_edge);
    iter1 = iter1->next;
  } while (iter1 != temp);

  // There MUST be at least one common edge between the two regions.
  assert(arrlen(v->common_edges));

  Edge *start = v->common_edges[0];
  Edge *end = v->common_edges[arrlen(v->common_edges) - 1];
  start->twin->prev->next = start->next;
  start->next->prev = start->twin->prev;

  start->prev->next = start->twin->next;
  start->twin->next->prev = start->prev;

  end->prev->next = end->twin->next;
  end->twin->next->prev = end->prev;

  end->twin->prev->next = end->next;
  end->next->prev = end->twin->prev;

  r1->incident_edge = start->prev;

  Edge *iter = r1->incident_edge;

  do {
    iter->region = r1;
    iter = iter->next;
  } while (iter != r1->incident_edge);

  int last_index = arrlen(v->region_list)-1; 
  // This should ALWAYS be the case since the newly
  // created region is deleted at the very begining
  assert( v->region_list[last_index] == r2 );
  arrdelswap( v->region_list, last_index );

  free(r2);
  for (int i = 0; i < arrlen(v->common_edges); i++) {
    free(v->common_edges[i]->twin);
    free(v->common_edges[i]);
  }
}

/* Delete the region given by r */
Region **internal_delete_region(Region **list, Region *r) {
  for (int i = 0; i < arrlen(list); i++) {
    if (list[i] == r) {
      arrdelswap(list, i);
    }
  }
  return list;
}

void find_region_intersection(Vonoroi *v, Region *r, const Line *l,
                              Edge *intersection_edge[]) {
  Edge *iter = r->incident_edge;
  int len = 0;
  do {
    v2 p1 = v->vertices[iter->vindex].point;
    v2 p2 = v->vertices[iter->twin->vindex].point;
    LineSegment segment = {.start = p1, .end = p2};
    v2 p;
    if (line_line_segment_intersect(l, &segment, &p)) {
      split_edges(v, iter, p);
      iter = iter->next;
      intersection_edge[len++] = iter;
    }
    iter = iter->next;
  } while (iter != r->incident_edge);
  assert(len == 2);
  return;
}

Edge *find_single_intersection(Vonoroi *v, Line *l, Edge *start,
                               v2 *intersection_point) {
  v2 new_intersection_point;
  Edge *eiter1 = start;
  do {
    v2 p1 = v->vertices[eiter1->vindex].point;
    v2 p2 = v->vertices[eiter1->twin->vindex].point;
    LineSegment segment = {.start = p1, .end = p2};
    if (line_line_segment_intersect(l, &segment, &new_intersection_point)) {
      split_edges(v, eiter1, new_intersection_point);
      *intersection_point = new_intersection_point;
      eiter1 = eiter1->next;
      return eiter1;
    }
    eiter1 = eiter1->next;

  } while (1);
  return NULL;
}

/* Insert a point in the vonoroi diagram */
// This need a HUGE refactor, but (somehow) works for now.
void insert_vonoroi_point(Vonoroi *v, v2 point) {
  // Find the vonoroi region for the point
  int index = find_vonoroi_region(v, point);
  assert(index != -1);

  v2 site = v->sites[index];
  Region *r = v->region_list[index];
  arrpush(v->sites, point);

  Line l = perpendicular_bisector(point, site);
  Edge *intersections[2];
  // Find the intersections of perpendicular bisector between
  // new point and the region point with the region boundary.
  find_region_intersection(v, r, &l, intersections);

  Region *new_region = split_region(v, r, point, intersections);
  arrsetlen(v->neighboring_regions, 0);
  {
    Edge *iter = new_region->incident_edge;
    do {
      Region *temp = iter->twin->region;
      if (temp && temp != r) {
        arrpush(v->neighboring_regions, iter->twin->region);
      }
      iter = iter->next;
    } while (iter != new_region->incident_edge);
  };

  Edge *eiter = intersections[0]->twin;
  v2 intersection_point = v->vertices[intersections[0]->vindex].point;
  Edge *new_intersections[2] = {[0] = intersections[0]->twin->next};
  Region *riter = intersections[0]->twin->region;
  Region *current_region = new_region;

  // Iterate over neighboring regions that start from the intersection
  // given by intersections[0].
  do {
    if (!riter) {
      break;
    }
    Region *r = riter;
    v2 rp1 = r->site;
    delete_region(v->neighboring_regions, riter);
    v2 new_intersection_point;
    Line perp = perpendicular_bisector(point, rp1);
    perp.p = intersection_point;
    new_intersections[1] = find_single_intersection(v, &perp, eiter->next->next,
                                                    &new_intersection_point);

    Region *new_r = split_region(v, riter, point, new_intersections);
    riter = new_intersections[1]->twin->region;
    intersection_point = new_intersection_point;
    eiter = new_intersections[1]->twin;
    new_intersections[0] = new_intersections[1]->twin->next;

    merge_regions(v, current_region, new_r);
  } while (riter && riter != intersections[1]->twin->region);

  // We returned back to the negihbor of intersections[1], i.e, we
  // were successfully able to complete the loop.
  if (riter) {
    // We were able to successfully reach the final region.
    intersections[0] = intersections[1]->twin->next;
    assert(new_intersections[1]);
    intersections[1] = new_intersections[1]->twin->next;
    assert(intersections[0]->region == intersections[1]->region);
    Region *new_r = split_region(v, riter, point, intersections);
    delete_region(v->neighboring_regions, riter);
    merge_regions(v, current_region, new_r);
  } else {
    // We hit the boundary before completing the loop.
    // No neighboring region along this edge
    riter = intersections[1]->twin->region;
    eiter = intersections[1]->twin;
    intersection_point = v->vertices[intersections[1]->vindex].point;
    new_intersections[0] = intersections[1]->twin->next;

    while (riter != NULL) {
      delete_region(v->neighboring_regions, riter);
      Region *r = riter;
      v2 rp1 = r->site;

      Line perp = perpendicular_bisector(point, rp1);
      perp.p = intersection_point;
      v2 new_intersection_point;

      new_intersections[1] = find_single_intersection(
          v, &perp, eiter->next->next, &new_intersection_point);

      Region *new_r = split_region(v, riter, point, new_intersections);
      riter = new_intersections[1]->twin->region;
      intersection_point = new_intersection_point;
      eiter = new_intersections[1]->twin;
      new_intersections[0] = new_intersections[1]->twin->next;
      merge_regions(v, current_region, new_r);
    }
  }

  // Wierd edge case in which (maybe) ONE of the region is left out.
  if (arrlen(v->neighboring_regions) > 0) {
    // Not sure if this is always the case. Needs more testing.
    assert(arrlen(v->neighboring_regions) == 1);
    Region *r = v->neighboring_regions[0];
    v2 rp1 = r->site;
    Line perp = perpendicular_bisector(point, rp1);
    find_region_intersection(v, r, &perp, intersections);
    Region *new_r = split_region(v, r, point, intersections);
    merge_regions(v, current_region, new_r);
  }
}

Vonoroi construct_vonoroi(void) {
  v2 initial_point = {INITIAL_SITE_X, INITIAL_SITE_Y};
  Vonoroi von = {0};

  arrsetcap(von.point_render_buffer, 256);
  arrpush(von.sites, initial_point);

  arrpush(von.vertices,
          create_vertex((v2){BOUNDING_BOX_XMIN, BOUNDING_BOX_YMIN}));
  arrpush(von.vertices,
          create_vertex((v2){BOUNDING_BOX_XMIN, BOUNDING_BOX_YMAX}));
  arrpush(von.vertices,
          create_vertex((v2){BOUNDING_BOX_XMAX, BOUNDING_BOX_YMAX}));
  arrpush(von.vertices,
          create_vertex((v2){BOUNDING_BOX_XMAX, BOUNDING_BOX_YMIN}));

  Edge *e = create_edge(0);
  e->twin = create_edge(1);
  e->twin->region = NULL;
  e->twin->twin = e;

  von.vertices[0].inc_edge = e;

  Region *new_region = xmalloc(sizeof(*new_region));

  new_region->incident_edge = e;
  new_region->site = initial_point;
  e->region = new_region;
  arrpush(von.region_list, new_region);

  Edge *iter = e;
  for (int i = 1; i < arrlenu(von.vertices); i++) {
    von.vertices[i].inc_edge = iter->next;
    iter->region = new_region;

    Edge *new = create_edge(i);
    Edge *new_twin = create_edge((i + 1) % arrlenu(von.vertices));
    new->twin = new_twin;
    new_twin->twin = new;
    new_twin->region = NULL;

    new->prev = iter;
    iter->next = new;

    new_twin->next = iter->twin;
    iter->twin->prev = new_twin;

    iter = iter->next;
  }
  iter->next = e;
  iter->twin->prev = e->twin;

  e->prev = iter;
  e->twin->next = iter->twin;

  return von;
}

int draw_line_segment(SDL_Renderer *r, LineSegment *l) {
  SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
  return SDL_RenderDrawLineF(r, l->start.x, l->start.y, l->end.x, l->end.y);
}

void render_vonoroi(SDL_Renderer *r, Vonoroi *v) {

  for (int i = 0; i < arrlenu(v->sites); i++) {
    v2 site = v->sites[i];
    SDL_FRect rect = {.x = site.x - (SITE_RECT_W / 2),
                      .y = site.y - (SITE_RECT_H / 2),
                      .w = SITE_RECT_W,
                      .h = SITE_RECT_H};
    SDL_SetRenderDrawColor(r, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRectF(r, &rect);
  }
  for (int i = 0; i < arrlenu(v->region_list); i++) {
    Region *region = v->region_list[i];
    Edge *iter = region->incident_edge;
    arrsetlen(v->point_render_buffer, 0);
    do {
      v2 p1 = v->vertices[iter->vindex].point;
      arrpush(v->point_render_buffer, p1);
      iter = iter->next;
    } while (iter != region->incident_edge);
    arrpush(v->point_render_buffer, v->vertices[iter->vindex].point);
    SDL_RenderDrawLinesF(r, (SDL_FPoint *)(v->point_render_buffer),
                         arrlen(v->point_render_buffer));
  }

#if 1
  for (int i = 0; i < arrlen(v->debug_lines); i++) {
    draw_line_segment(r, v->debug_lines + i);
  }
#endif
}

void destroy_vonoroi(Vonoroi *v) {
  for (int i = 0; i < arrlen(v->region_list); i++) {
    Edge *iter = v->region_list[i]->incident_edge;
    do {
      if (iter->twin) {
        iter->twin->twin = NULL;
        if (iter->twin->region == NULL) {
          free(iter->twin);
        }
      }
      Edge *nextptr = iter->next;

      free(iter);
      iter = nextptr;
    } while (iter != v->region_list[i]->incident_edge);
    free(v->region_list[i]);
  }
  arrfree(v->common_edges);
  arrfree(v->vertices);
  arrfree(v->debug_lines);
  arrfree(v->sites);
  arrfree(v->region_list);
  arrfree(v->neighboring_regions);
  arrfree(v->point_render_buffer);
}

int main(int argc, char *argv[]) {

  if (INITIAL_SITE_X < BOUNDING_BOX_XMIN ||
      INITIAL_SITE_X > BOUNDING_BOX_XMAX ||
      INITIAL_SITE_Y < BOUNDING_BOX_YMIN ||
      INITIAL_SITE_Y > BOUNDING_BOX_YMAX) {
    fprintf(stderr, "Invalid values of INITIAL_SITE_X and INITIAL_SITE_Y for "
                    "the given bounding box.\n");
    fprintf(stderr, "Values are: INITIAL_SITE_X  = %d, INITIAL_SITE_Y = %d",
            INITIAL_SITE_X, INITIAL_SITE_Y);
    fprintf(stderr,
            "Bounding box is: (xmin,ymin)=(%d,%d), (xmax,ymax)=(%d,%d)\n",
            BOUNDING_BOX_XMIN, BOUNDING_BOX_YMIN, BOUNDING_BOX_XMAX,
            BOUNDING_BOX_YMAX);
    return 0;
  }
  Vonoroi v = construct_vonoroi();
  SDL_Renderer *renderer;
#if 1
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL could not initialize: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow("Vonoroi", 0, 0, SCREEN_WIDTH,
                                        SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "Failed to initialize SDL Window! %s\n", SDL_GetError());
    return -1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "Failed to create renderer! %s\n", SDL_GetError());
    return -1;
  }

  bool running = true;
  int count = 1;
  SDL_Event event;
  while (running) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
        break;

      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          int x = event.button.x;
          int y = event.button.y;
          if (x > BOUNDING_BOX_XMIN && x < BOUNDING_BOX_XMAX &&
              y > BOUNDING_BOX_YMIN && y < BOUNDING_BOX_YMAX) {
            count += 1;
            printf("Count = %d, (x,y)=(%d,%d)\n", count, x, y);
            insert_vonoroi_point(&v, (v2){x, y});
          }
        }
        break;
      }
    }
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    render_vonoroi(renderer, &v);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
#endif
  destroy_vonoroi(&v);
  return 1;
}
