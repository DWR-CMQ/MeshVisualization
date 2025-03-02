#ifndef EASY3D_CORE_MODEL_H
#define EASY3D_CORE_MODEL_H


#include <string>
#include <vector>

#include "types.h"


namespace MV {

    class Renderer;
    class Manipulator;

    /**
     * \brief The base class of renderable 3D models.
     * \class Model MV/core/model.h
     * \details A model can represent a SurfaceMesh, a PointCloud, or a Graph. A model can have multiple drawables,
     * e.g., faces, vertices, edges.
     */
    class Model
    {
    public:
        /// \brief Default constructor.
        /// The parameter \p name is optional, but it is useful for handling multiple models with the same viewer.
        explicit Model(const std::string& name = "unknown");
        virtual ~Model() = default;

        /// \brief Sets/Changes the name of a model.
        /// Assigning a name to a model is optional, but it is useful for handling multiple models with the same viewer.
        void set_name(const std::string& n) { name_ = n; }
        /// \brief The name of a model.
        const std::string& name() const { return name_; }

        /**
         * \brief The bounding box of the model.
         * \param recompute If \c true, or if the bounding box is not known, it computes and returns the bounding
         *      box of the model. Otherwise, it returns the known bounding box.
         * \note Manipulation transformation is not handled.
         * \see invalidate_bounding_box().
         */
        const Box3& bounding_box(bool recompute = false) const;

        /**
         * \brief Invalidates the bounding box of the model. So when bounding_box() is called, the bounding box will
         * be re-computed. This function is typically called when the geometry of a model is changed.
         */
        void invalidate_bounding_box();

        /** \brief The vertices of the model. */
        virtual std::vector<vec3>& points() = 0;
        /** \brief The vertices of the model. */
        virtual const std::vector<vec3>& points() const = 0;

        /** \brief Tests if the model is empty. */
        bool empty() const { return points().empty(); };

        /** \brief Prints the names of all properties to an output stream (e.g., std::cout). */
        virtual void property_stats(std::ostream &output) const {}

        /**
         * \brief Sets the renderer of this model.
         * \note Memory management of the renderer is the user's responsibility.
         */
        void set_renderer(Renderer* r) { renderer_ = r; }
        /** \brief Gets the renderer of this model. */
        Renderer* renderer();
        /** \brief Gets the constant renderer of this model. */
        const Renderer* renderer() const;

        /**
         * \brief Attaches a manipulator to this model.
         * \note Memory management of the manipulator is the user's responsibility.
         */
        void set_manipulator(Manipulator* manip) { manipulator_ = manip; }

        /** \brief Gets the manipulator attached to this model. */
        Manipulator* manipulator() { return manipulator_; }
        /** \brief Gets the manipulator attached to this model. */
        const Manipulator* manipulator() const { return manipulator_; }

    protected:
        std::string	name_;

        Box3		bbox_;
        bool		bbox_known_;

        Renderer* renderer_;         // for rendering
        Manipulator* manipulator_;   // for manipulation
    };
}

#endif  // EASY3D_CORE_MODEL_H
