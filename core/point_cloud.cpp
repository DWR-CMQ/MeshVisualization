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

#include "point_cloud.h"

namespace MV {

    PointCloud::PointCloud()
    {
        // allocate standard properties
        // same list is used in operator=() and assign()
        m_vpoint   = add_vertex_property<vec3>("v:point");
        m_vdeleted = add_vertex_property<bool>("v:deleted", false);

        m_mprops.push_back();

        m_uideletedvertices = 0;
        m_bgarbage = false;
    }


    //-----------------------------------------------------------------------------


    PointCloud& PointCloud::operator=(const PointCloud& rhs)
    {
        if (this != &rhs)
        {
            // deep copy of property containers
            m_vprops = rhs.m_vprops;
            m_mprops = rhs.m_mprops;

            // property handles contain pointers, have to be reassigned
            m_vdeleted = vertex_property<bool>("v:deleted");
            m_vpoint   = vertex_property<vec3>("v:point");

            // how many elements are deleted?
            m_uideletedvertices = rhs.m_uideletedvertices;
            m_bgarbage          = rhs.m_bgarbage;
        }

        return *this;
    }


	//-----------------------------------------------------------------------------


	PointCloud &PointCloud::join(const PointCloud &other) {
		// increase capacity
		resize(vertices_size() + other.vertices_size());

		// append properties in the free space created by resize
		m_vprops.transfer(other.m_vprops);

        // update garbage infos
		m_bgarbage = m_bgarbage || other.m_bgarbage;
		m_uideletedvertices += other.m_uideletedvertices;
		return *this;
	}


    //-----------------------------------------------------------------------------


    PointCloud& PointCloud::assign(const PointCloud& rhs)
    {
        if (this != &rhs)
        {
            // clear properties
            m_vprops.clear();
            m_mprops.clear();

            // allocate standard properties
            m_vpoint   = add_vertex_property<vec3>("v:point");
            m_vdeleted = add_vertex_property<bool>("v:deleted", false);

            // copy properties from other cloud
            m_vpoint.array() = rhs.m_vpoint.array();
            m_vdeleted.array() = rhs.m_vdeleted.array();

            // resize (needed by property containers)
            m_vprops.resize(rhs.vertices_size());
            m_mprops.resize(1);

            // how many elements are deleted?
            m_uideletedvertices = rhs.m_uideletedvertices;
            m_bgarbage          = rhs.m_bgarbage;
        }

        return *this;
    }


    //-----------------------------------------------------------------------------


    void PointCloud::clear()
    {
        //---- clear without removing properties

        m_vprops.resize(0);

        m_vprops.shrink_to_fit();

        m_uideletedvertices = 0;
        m_bgarbage = false;

        //---- keep the standard properties and remove all the other properties

        m_vprops.resize_property_array(2);   // "v:point", "v:deleted"
        m_mprops.clear();
        m_mprops.resize(1);
    }


    //-----------------------------------------------------------------------------


    void PointCloud::property_stats(std::ostream& output) const
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

		props = model_properties();
		if (!props.empty())
		{
			std::cout << "model properties:\n";
            for (const auto& p : props)
                output << "\t" << p << std::endl;
		}
    }


    //-----------------------------------------------------------------------------


    PointCloud::Vertex PointCloud::add_vertex(const vec3& p)
    {
        Vertex v = new_vertex();
        m_vpoint[v] = p;
        return v;
    }


    //-----------------------------------------------------------------------------


    void PointCloud::delete_vertex(Vertex v)
    {
        if (m_vdeleted[v])  return;

        // mark v as deleted
        m_vdeleted[v] = true;
        m_uideletedvertices++;
        m_bgarbage = true;
    }


    //-----------------------------------------------------------------------------


    void PointCloud::collect_garbage()
    {
        int  nV = static_cast<int>(vertices_size());

        // setup handle mapping
        auto vmap = add_vertex_property<Vertex>("v:garbage-collection");
        for (int i=0; i<nV; ++i)
            vmap[Vertex(i)] = Vertex(i);

        // remove deleted vertices
        if (nV > 0)
        {
            int i0 = 0;
            int i1 = nV - 1;

            while (true)
            {
                // find first deleted and last un-deleted
                while (!m_vdeleted[Vertex(i0)] && i0 < i1)  ++i0;
                while ( m_vdeleted[Vertex(i1)] && i0 < i1)  --i1;
                if (i0 >= i1) break;

                // swap
                m_vprops.swap(i0, i1);
            }

            // remember new size
            nV = m_vdeleted[Vertex(i0)] ? i0 : i0+1;
        }

        // remove handle maps
        remove_vertex_property(vmap);

        // finally resize arrays
        m_vprops.resize(nV);
        m_vprops.shrink_to_fit();

        m_uideletedvertices = 0;
        m_bgarbage = false;
    }

} // namespace MV
