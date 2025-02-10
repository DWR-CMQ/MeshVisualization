/********************************************************************
 * Copyright (C) 2015 Liangliang Nan <liangliang.nan@gmail.com>
 * https://3d.bk.tudelft.nl/liangliang/
 *
 * This file is part of Easy3D. If it is useful in your research/work,
 * I would be grateful if you show your appreciation by citing it:
 * ------------------------------------------------------------------
 *      Liangliang Nan.
 *      Easy3D: a lightweight, easy-to-use, and efficient C++ library
 *      for processing and rendering 3D data.
 *      Journal of Open Source Software, 6(64), 3255, 2021.
 * ------------------------------------------------------------------
 *
 * Easy3D is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3
 * as published by the Free Software Foundation.
 *
 * Easy3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************/

#include "graph.h"


namespace MV {


    Graph::Graph()
    {
        // allocate standard properties
        // same list is used in operator=() and assign()
        m_vconn    = add_vertex_property<VertexConnectivity>("v:connectivity");
        m_econn    = add_edge_property<EdgeConnectivity>("e:connectivity");
        m_vpoint   = add_vertex_property<vec3>("v:point");
        m_vdeleted = add_vertex_property<bool>("v:deleted", false);
        m_edeleted = add_edge_property<bool>("e:deleted", false);

        m_mprops.push_back();

        m_uideletedvertices = m_uideletededges = 0;
        m_bgarbage = false;
    }


    //-----------------------------------------------------------------------------


    Graph& Graph::operator=(const Graph& rhs)
    {
        if (this != &rhs)
        {
            // deep copy of property containers
            m_vprops = rhs.m_vprops;
            m_eprops = rhs.m_eprops;
            m_mprops = rhs.m_mprops;

            // property handles contain pointers, have to be reassigned
            m_vconn    = vertex_property<VertexConnectivity>("v:connectivity");
            m_econn    = edge_property<EdgeConnectivity>("e:connectivity");
            m_vdeleted = vertex_property<bool>("v:deleted");
            m_edeleted = edge_property<bool>("e:deleted");
            m_vpoint   = vertex_property<vec3>("v:point");

            // how many elements are deleted?
            m_uideletedvertices = rhs.m_uideletedvertices;
            m_uideletededges    = rhs.m_uideletededges;
            m_bgarbage          = rhs.m_bgarbage;
        }

        return *this;
    }


    //-----------------------------------------------------------------------------


    Graph& Graph::assign(const Graph& rhs)
    {
        if (this != &rhs)
        {
            // clear properties
            m_vprops.clear();
            m_eprops.clear();
            m_mprops.clear();

            // allocate standard properties
            m_vconn    = add_vertex_property<VertexConnectivity>("v:connectivity");
            m_econn    = add_edge_property<EdgeConnectivity>("e:connectivity");
            m_vpoint   = add_vertex_property<vec3>("v:point");
            m_vdeleted = add_vertex_property<bool>("v:deleted", false);
            m_edeleted = add_edge_property<bool>("e:deleted", false);

            // copy properties from other graph
            m_vconn.array()     = rhs.m_vconn.array();
            m_econn.array()     = rhs.m_econn.array();
            m_vpoint.array()    = rhs.m_vpoint.array();
            m_vdeleted.array()  = rhs.m_vdeleted.array();
            m_edeleted.array()  = rhs.m_edeleted.array();

            // resize (needed by property containers)
            m_vprops.resize(rhs.vertices_size());
            m_eprops.resize(rhs.edges_size());
            m_mprops.resize(1);

            // how many elements are deleted?
            m_uideletedvertices = rhs.m_uideletedvertices;
            m_uideletededges    = rhs.m_uideletededges;
            m_bgarbage          = rhs.m_bgarbage;
        }

        return *this;
    }


    //-----------------------------------------------------------------------------


    void Graph::clear()
    {
        //---- clear without removing properties

        m_vprops.resize(0);
        m_eprops.resize(0);
        m_mprops.resize(0);

        m_vprops.shrink_to_fit();
        m_eprops.shrink_to_fit();
        m_mprops.shrink_to_fit();

        m_uideletedvertices = m_uideletededges = 0;
        m_bgarbage = false;

        //---- keep the standard properties and remove all the other properties

        m_vprops.resize_property_array(3);   // "v:connectivity", "v:point", "v:deleted"
        m_eprops.resize_property_array(2);   // "e:connectivity", "e:deleted"

        m_mprops.clear();
        m_mprops.resize(1);
    }


    //-----------------------------------------------------------------------------


    void Graph::reserve(unsigned int nvertices, unsigned int nedges)
    {
        m_vprops.reserve(nvertices);
        m_eprops.reserve(nedges);
        m_mprops.reserve(1);
    }


    //-----------------------------------------------------------------------------


    void Graph::property_stats(std::ostream& output) const
    {
        std::vector<std::string> props;

        props = model_properties();
        if (!props.empty())
        {
            output << "model properties:\n";
            for (const auto& p : props)
                output << "\t" << p << std::endl;
        }

		props = vertex_properties();
		if (!props.empty())
		{
			std::cout << "vertex properties:\n";
            for (const auto& p : props)
                output << "\t" << p << std::endl;
		}


        props = edge_properties();
		if (!props.empty())
		{
			std::cout << "edge properties:\n";
            for (const auto& p : props)
                output << "\t" << p << std::endl;
		}

		props = model_properties();
		if (!props.empty())
		{
			std::cout << "model properties:\n";
            for (const auto& p : props)
                output << "\t" << p << std::endl;
		}
    }


    //-----------------------------------------------------------------------------


    Graph::Vertex Graph:: add_vertex(const vec3& p)
    {
        Vertex v = new_vertex();
        m_vpoint[v] = p;
        return v;
    }


    Graph::Edge Graph::add_edge(const Vertex& start, const Vertex& end) {
        assert(start != end);
        auto e = find_edge(start, end);
        if (e.is_valid()) {
            LOG(WARNING) << "edge already exists [" << start << ", " <<  end << "]";
            return e;
        } else {
            e = new_edge();
            m_econn[e].source_ = start;
            m_econn[e].target_ = end;
            m_vconn[start].edges_.push_back(e);
            m_vconn[end].edges_.push_back(e);
            return e;
        }
    }


    //-----------------------------------------------------------------------------


    Graph::Edge Graph::find_edge(Vertex start, Vertex end) const
    {
        assert(is_valid(start) && is_valid(end));

        for (auto e : edges(start)) {
            if (vertex(e, 0) == end || vertex(e, 1) == end)
                return e;
        }

        return Edge();
    }


    //-----------------------------------------------------------------------------


    unsigned int Graph::valence(Vertex v) const
    {
        return static_cast<unsigned int>(m_vconn[v].edges_.size());
    }


    //-----------------------------------------------------------------------------

    float Graph::edge_length(Edge e) const
    {
        return norm(m_vpoint[vertex(e, 0)] - m_vpoint[vertex(e, 1)]);
    }


    //-----------------------------------------------------------------------------

    void Graph::delete_vertex(Vertex v)
    {
        /// todo: implement this function
        LOG(WARNING) << "not implemented. Please ask Liangliang (liangliang.nan@gmail.com) to implement this function";
    //    if (m_vdeleted[v])  return;

    //    // collect incident faces
    //    std::vector<Face> incident_faces;
    //    incident_faces.reserve(6);

    //    FaceAroundVertexCirculator fc, fc_end;
    //    fc = fc_end = faces(v);

    //    if (fc)
    //    do
    //    {
    //        incident_faces.push_back(*fc);
    //    } while (++fc != fc_end);

    //    // delete incident faces
    //    std::vector<Face>::iterator fit(incident_faces.begin()),
    //                                fend(incident_faces.end());

    //    for (; fit != fend; ++fit)
    //        delete_face(*fit);

    //    // mark v as deleted if not yet done by delete_face()
    //    if (!m_vdeleted[v])
    //    {
    //        m_vdeleted[v] = true;
    //        m_uideletedvertices++;
    //        m_bgarbage = true;
    //    }
    }


    //-----------------------------------------------------------------------------


    void Graph::delete_edge(Edge e)
    {
        /// todo: implement this function
        LOG(WARNING) << "not implemented. Please ask Liangliang (liangliang.nan@gmail.com) to implement this function";
    //    if (m_edeleted[e])  return;

    //    Face f0 = face(halfedge(e, 0));
    //    Face f1 = face(halfedge(e, 1));

    //    if (f0.is_valid()) delete_face(f0);
    //    if (f1.is_valid()) delete_face(f1);
    }


    //-----------------------------------------------------------------------------

    void Graph::collect_garbage()
    {
        /// todo: implement this function
        LOG(WARNING) << "not implemented. Please ask Liangliang (liangliang.nan@gmail.com) to implement this function";
    //    int  i, i0, i1,
    //    nV(vertices_size()),
    //    nE(edges_size()),
    //    nH(halfedges_size()),

    //    Vertex    v;
    //    Halfedge  h;


    //    // setup handle mapping
    //    VertexProperty<Vertex>      vmap = add_vertex_property<Vertex>("v:garbage-collection");
    //    EdgeProperty<Edge>          emap = add_edge_property<Edge>("e:garbage-collection");

    //    for (i=0; i<nV; ++i)
    //        vmap[Vertex(i)] = Vertex(i);
    //    for (i=0; i<nH; ++i)
    //        emap[Edge(i)] = Edge(i);

    //    // remove deleted vertices
    //    if (nV > 0)
    //    {
    //        i0=0;  i1=nV-1;

    //        while (1)
    //        {
    //            // find first deleted and last un-deleted
    //            while (!m_vdeleted[Vertex(i0)] && i0 < i1)  ++i0;
    //            while ( m_vdeleted[Vertex(i1)] && i0 < i1)  --i1;
    //            if (i0 >= i1) break;

    //            // swap
    //            m_vprops.swap(i0, i1);
    //        };

    //        // remember new size
    //        nV = m_vdeleted[Vertex(i0)] ? i0 : i0+1;
    //    }


    //    // remove deleted edges
    //    if (nE > 0)
    //    {
    //        i0=0;  i1=nE-1;

    //        while (1)
    //        {
    //            // find first deleted and last un-deleted
    //            while (!m_edeleted[Edge(i0)] && i0 < i1)  ++i0;
    //            while ( m_edeleted[Edge(i1)] && i0 < i1)  --i1;
    //            if (i0 >= i1) break;

    //            // swap
    //            m_eprops.swap(i0, i1);
    //            m_eprops.swap(2*i0,   2*i1);
    //        };

    //        // remember new size
    //        nE = m_edeleted[Edge(i0)] ? i0 : i0+1;
    //        nH = 2*nE;
    //    }

    //    // update vertex connectivity
    //    for (i=0; i<nV; ++i)
    //    {
    //        v = Vertex(i);
    //        if (!is_isolated(v))
    //            set_halfedge(v, hmap[edges(v)]);
    //    }


    //    // update halfedge connectivity
    //    for (i=0; i<nH; ++i)
    //    {
    //        h = Halfedge(i);
    //        set_vertex(h, vmap[target(h)]);
    //        set_next(h, hmap[next(h)]);
    //        if (!is_border(h))
    //            set_face(h, fmap[face(h)]);
    //    }

    //    // remove handle maps
    //    remove_vertex_property(vmap);
    //    remove_edge_property(emap);


    //    // finally resize arrays
    //    m_vprops.resize(nV); m_vprops.free_memory();
    //    m_eprops.resize(nH); m_eprops.free_memory();

    //    m_uideletedvertices = m_uideletededges = 0;
    //    m_bgarbage = false;
    }


//    std::vector<Graph::Vertex> Graph::vertices(Vertex v) const {
//        assert(v.is_valid());
//        std::vector<Graph::Vertex> result;
//        for (auto e : edges(v)) {
//            Vertex another = source(e);
//            if (another != v)
//                result.push_back(another);
//            else
//                result.push_back(target(e));
//        }

//        return result;
//    }


} // namespace MV
