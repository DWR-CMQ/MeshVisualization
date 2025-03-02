﻿/********************************************************************
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

#ifndef EASY3D_RENDERER_FRAMEBUFFER_OBJECT_H
#define EASY3D_RENDERER_FRAMEBUFFER_OBJECT_H


#include <string>
#include <vector>

#include "opengl.h"

namespace MV {

    /**
     * \brief An implementation of framebuffer object (FBO).
     * \class FramebufferObject MV/renderer/framebuffer_object.h
     * \details A framebuffer object is conceptually a structure containing pointers to GPU memory.
     *      The memory pointed to is either an OpenGL texture or an OpenGL RenderBuffer. FBOs can be
     *      used to render to one or more textures, share depth buffers between multiple sets of
     *      color buffers/textures.
     *      http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt for details.
     *
     *      Note that you need to create a FramebufferObject with more than one sample per pixel for
     *      primitives to be antialiased. To create a multisample framebuffer object you should set
     *      the sample property to a non-zero value. The default sample count of 0 represents a regular
     *      non-multisample framebuffer object. If the desired amount of samples per pixel is not
     *      supported by the hardware then the maximum number of samples per pixel will be used. The
     *      GL_EXT_framebuffer_multisample extension is required to create a framebuffer with more than
     *      one sample per pixel.
     *
     * \note
     *      - A valid OpenGL context must be present when creating a FramebufferObject, otherwise initialization will fail.
     *      - GL_TEXTURE_2D textures must have a power of 2 width and height(e.g. 256x512), unless you are using OpenGL 2.0
     *        or higher.
     *      - To create a multisample framebuffer object you should set the sample property to a non-zero value.
     *      - If you want to use a multisample framebuffer object a texture, you need to blit it to a regular framebuffer
     *        object using blit_framebuffer().
     *      - It is more efficient(but not required) to call bind() on an FBO before making multiple method calls.
     *        For example :
     * 		        \code
     * 				    FramebufferObject fbo;
     * 				    fbo.bind();
     * 				    fbo.add_color_buffer();
     * 				    fbo.add_color_buffer();
     * 				    fbo.print_attachments();
     * 				\endcode
     * 			To provide a complete encapsulation, the following usage pattern works
     * 			correctly but is less efficient :
     * 			    \code
     * 				    FramebufferObject fbo;
     * 				    // NOTE: No bind() call
     * 				    fbo.add_color_buffer();
     * 				    fbo.add_color_buffer();
     * 				    fbo.print_attachments();
     * 				\endcode
     *  		The first usage pattern binds the FBO only once, whereas the second usage binds/unbinds the FBO for
     *  		each method call.
     *
     * Example usage 1: draw to a fbo:
     *      \code
     * 		    FramebufferObject fbo(w, h, 0);
     * 		    fbo.add_color_texture();
     * 		    fbo.add_depth_buffer();
     * 		    fbo.bind();
     * 		    fbo.activate_draw_buffer(0);
     * 		    glClearDepth(1.0f);								// optional, done by default
     * 		    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);				// optional, done by default
     * 		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// optional, done by default
     * 		    you draw code here...
     * 		    fbo.release();
     *          //... Now you can use fbo.color_texture() for other purposes.
     *      \endcode
     *
     *  Example usage 2: draw the depth map of an object or a scene:
     *      \code
     * 		    FramebufferObject fbo(w, h, 0);
     * 		    fbo.add_depth_texture();
     * 		    fbo.bind();
     * 		    glClearDepth(1.0f);				// optional, done by default
     * 		    glClear(GL_DEPTH_BUFFER_BIT);	    // optional, done by default
     * 		    you draw code here...
     * 		    fbo.release();
     *          //... Now you can use the fbo.depth_texture().
     *      \endcode
     * For both the above examples, it's easy to enable MSAA by creating a multisample FBO and render to it.
     * But do remember to blit to a normal FBO before using the   texture.
     *
     * Current implement support only GL_TEXTURE_2D (see the texture_target_ variable), and it does not manage
     * externally created textures.
     * \todo
     *   - add a class TextureFormat for flexible format specification and add related
     *     functions using the TextureFormat class.
     *   - modify the attach_color_texture() and attach_depth_texture() to use TextureFormat.
     *   - replace ColorAttachment by TextureFormat.
     */
    class FramebufferObject
    {
    public:
        /// Queries if FramebufferObject is supported.
        static bool is_supported();

        /// Default constructor.
        FramebufferObject(int w, int h, int samples = 0);
        /// Destructor.
        virtual ~FramebufferObject();

        //////////////////////////////////////////////////////////////////////////

        /// Add a color texture render buffer.
        bool add_color_texture(
            GLenum internal_format = GL_RGBA8,	// GL_[components​][size​][type​], e.g., GL_RG8, GL_RGBA16, GL_R16F, GL_RG16, GL_RGBA32F ...
            GLenum format = GL_RGBA,			// The format of the pixel data (GL_RED, GL_RG, GL_RGB, GL_BGR, GL_BGRA ...)
            GLenum type = GL_UNSIGNED_BYTE,		// The data type of the pixel data (GL_BYTE, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT ...)
            GLenum filter = GL_NEAREST			// The texture minifying/magnification functions. e.g., GL_NEAREST, GL_LINEAR
        );
        /// Add a color render buffer.
        bool add_color_buffer(
            GLenum internal_format = GL_RGBA8,	// GL_[components​][size​][type​], e.g., GL_RG8, GL_RGBA16, GL_R16F, GL_RG16, GL_RGBA32F ...
            GLenum format = GL_RGBA,			// The format of the pixel data (GL_RED, GL_RG, GL_RGB, GL_BGR, GL_BGRA ...)
            GLenum type = GL_UNSIGNED_BYTE		// The data type of the pixel data (GL_BYTE, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT ...)
        );
        /// Add a depth texture render buffer.
        bool add_depth_texture(
            GLenum internal_format = GL_DEPTH24_STENCIL8,	// GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32F, GL_DEPTH24_STENCIL8, GL_DEPTH32F_STENCIL8, GL_DEPTH_COMPONENT16
            GLenum filter = GL_NEAREST,						// The texture minifying/magnification functions. e.g., GL_NEAREST, GL_LINEAR
            GLenum compare_mode = GL_NONE,					// GL_COMPARE_REF_TO_TEXTURE.
            GLenum compare_func = GL_LEQUAL					// GL_GEQUAL, GL_LESS, GL_GREATER, GL_EQUAL, GL_NOTEQUAL, GL_ALWAYS(for 1.0) and GL_NEVER(for 0.0).
        );
        /// Add a depth render buffer.
        bool add_depth_buffer(
            GLenum internal_format = GL_DEPTH24_STENCIL8	// GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32F, GL_DEPTH24_STENCIL8, GL_DEPTH32F_STENCIL8, GL_DEPTH_COMPONENT16
        );

		/// Attach an existing color texture to the framebuffer
		/// \param target GL_TEXTURE_2D, GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_ARRAY
		/// \param attachment GL_COLOR_ATTACHMENTi
		bool attach_color_texture(GLenum target, GLuint texture_id, GLenum attachment);

		/// Attach an existing depth texture to the framebuffer
		/// \param target GL_TEXTURE_2D, GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_ARRAY
		/// \param attachment GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT or GL_DEPTH_STENCIL_ATTACHMENT.
		bool attach_depth_texture(GLenum target, GLuint texture_id, GLenum attachment);

        //////////////////////////////////////////////////////////////////////////

        /// Ensure the size of the buffers is \p (w, h).
        /// If its size matches the required size, i.e, (w, h), it does nothing.
        /// If doesn't match, it clears and reallocate memory in the GPU.
        void ensure_size(int w, int h);

        /// Bind FBO to FRAMEBUFFER, DRAW_FRAMEBUFFER, or READ_FRAMEBUFFER
        /// The current binding is saved for later release.
        /// \note bind() and release() should be called in pair.
        bool bind(GLenum target = GL_FRAMEBUFFER);
        /// Switches rendering back to the default framebuffer.
        bool release(GLenum target = GL_FRAMEBUFFER);

        /// Returns true if the framebuffer object is valid.
        bool is_valid() const;
        /// Check if the framebuffer object is currently bound to the current context.
        bool is_bound(GLenum target = GL_FRAMEBUFFER) const;

        /// Choose the buffers to render into. This command lets you select which attachments are written to.
        /// \note The default buffer is the 0. In such a case calling to this function is optional.
        void activate_draw_buffer(unsigned int index) const;
        void activate_draw_buffers(unsigned int num_buffers, const unsigned int indices[]) const;
        void activate_draw_buffers(unsigned int minId, unsigned int maxId) const; // activate draw buffer in the range [minId, ... maxId]
        void deactivate_draw_buffers() const;

        /// Choose the buffers to read from. This command lets you select which attachment to read from.
        /// \note The default buffer is the 0. In such a case calling to this function is optional.
        void activate_read_buffer(unsigned int index) const;
        /// Deactivates reading from the buffers.
        void deactivate_read_buffer() const;

        /// Returns the OpenGL framebuffer object handle for this framebuffer object (returned by the glGenFramebuffers() function).
        /// This handle can be used to attach new images or buffers to the framebuffer. If you attach images or buffers, you are
        /// responsible for cleaning up and destroying these objects.
        GLuint handle() const { return fbo_id_; }

        /// Returns the width of the render buffers.
        int  width() const { return width_; }
        /// Returns the height of the render buffers.
        int  height() const { return height_; }

        /// The returned value can be greater than the requested value since the typically supported
        /// values are 0, 4, 8, ..., and the requests are mapped to the next supported value.
        int  samples() const { return samples_; }

        /// Returns the number of color attachments
        int	 num_color_attachments() const;
        /// Does the fbo have color attachment at \p index?
        bool has_color_attachment(unsigned int index) const;

        /// Does the fbo have a depth attachment.
        bool has_depth_attachment() const;
        /// Returns the depth bits.
        int  depth_bits() const;

        /// Does the fbo have a stencil buffer.
        bool has_stencil() const;

        /// Returns the texture target, i.e., GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE
        GLenum texture_target() const;

        /**
         * Returns the texture id attached to the color attachment \p index of this framebuffer object.
         * @param index The index of the color attachment.
         * @param resolve If a multisample framebuffer object is used, the function blits and returns the resolved
         *      non-multisample texture when \p resolve is true.
         * @return The ID of the color texture.
         */
        GLuint color_texture(unsigned int index = 0, bool resolve = true) const;
        /// Does the fbo have a color texture for color attachment \p index?
        bool   has_color_texture(unsigned int index) const;

        /**
         * Returns the texture id for the texture attached to the depth attachment of this framebuffer object.
         * @param resolve If a multisample framebuffer object is used, the function blits and returns the resolved
         *      non-multisample texture if \p resolve is true.
         * @return The ID of the depth texture.
         */
        GLuint depth_texture(bool resolve = true) const;
        /// Does the fbo have a depth texture?
        bool   has_depth_texture() const;

        /**
         * \brief Makes a copy of the current buffer into a texture (regardless the attachments already have textures).
         * \details Internally it creates a texture and uses glCopyTexSubImage2D() to directly copy the buffer in it.
         *
         * @param texture_handle The target texture (will be created if it does not exist).
         * @param index The index of the color attachment.
         * @param internalFormat, format, type Used to define the texture format and hence which and how components of the
         *      buffer are copied into the texture. See the glTexImage2D() documentation for details.
         * @param filter Specifies how the texture minifying and magnification function work.
         *
         * The typical internalFormat/format/type combinations are:
         *      For depth:	GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT24 / GL_DEPTH_COMPONENT / GL_UNSIGNED_BYTE
         *			        GL_DEPTH_COMPONENT32F / GL_DEPTH_COMPONENT / GL_FLOAT
         *	    For color: 	GL_RGBA8 / GL_RGBA / GL_UNSIGNED_BYTE
         *			        \p internalFormat mus be GL_[components][size][type], e.g., GL_RG8, GL_RGBA16, GL_R16F,
         *			           GL_RG16, GL_RGBA32F ...
         *                  \p format must be one of the GL_RED, GL_RG, GL_RGB, GL_BGR, GL_BGRA ...
         *					\p type can be GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT,
         *					   GL_INT, GL_FLOAT ...
         *	Use GL_LUMINANCE as the \p internalFormat and GL_RED, GL_GREEN or GL_BLUE as \p format to capture a single
         *	color component as a luminance (gray scaled) value.
         * @return \p true on success.
         * \note
         *      - GL_STENCIL is not supported as a format.
         *		- The GL_DEPTH_COMPONENT format may not be supported by all hardware. It may sometimes be emulated in
         *		  software, resulting in poor performances.
         *		- You need to release the texture when you're done.
         */
        bool copy_color_to_texture(
            GLuint& texture_handle,
            unsigned int index = 0,
            int internalFormat = GL_RGBA8,
            GLenum format = GL_RGBA,
            GLenum type = GL_UNSIGNED_BYTE,
            GLenum filter = GL_NEAREST
        );

        /**
         * \brief Makes a copy of the current depth buffer into a texture.
         * @param texture_handle The target texture (will be created if it does not exist).
         * @param internalFormat Used to define the texture format.
         * @param filter Specifies how the texture minifying and magnification function work.
         * @return \p true on success.
         * \note
         *		- You need to release the texture when you're done.
         */
        bool copy_depth_to_texture(
            GLuint& texture_handle,
            unsigned int internal_format = GL_DEPTH24_STENCIL8,
            GLenum filter = GL_NEAREST
        );

        /// Print all the attachments of the current framebuffer object.
        void print_attachments() const;
        /// Print the draw buffers.
        void print_draw_buffers() const;
        /// Print the read buffer.
        void print_read_buffer() const;

        /// Read the color render buffer attached to color attachment \p index.
        /// \param format The format of the pixel data. The following formats are accepted: GL_RGB, GL_BGR, GL_RGBA,
        ///         and GL_BGRA.
        bool read_color(unsigned int index, unsigned char* buffer, unsigned int format, bool flip_vertically = true) const;
        /// Read the color render buffer attached to color attachment \p index.
        /// \param format The format of the pixel data. The following formats are accepted: GL_RGB, GL_BGR, GL_RGBA,
        ///         and GL_BGRA.
        bool read_color(unsigned int index, std::vector<unsigned char>& buffer, unsigned int format, bool flip_vertically = true) const;

        /// Read the depth render buffer into a specified buffer.
        bool read_depth(float* buffer, bool flip_vertically = true) const;
        /// Read the depth render buffer into a specified buffer.
        bool read_depth(std::vector<float>& buffer, bool flip_vertically = true) const;

        /// Reads the color at pixel \p (x, y) from the color render buffer attached to color attachment \p index.
        /// Returns false if the color attachment index does not exist.
        /// \note \p (x, y) are in the OpenGL coordinate system.
        bool read_color(unsigned char rgba[4], int x, int y, int index = 0) const;

        /// Reads the depth at pixel \p (x, y).
        /// Returns \p false if the depth attachment does not exist.
        /// \note \p (x, y) are in the OpenGL coordinate system.
        bool read_depth(float& depth, int x, int y) const;

        /// Snapshots the color render buffer attached to color attachment \p index into an image file.
        /// \note Only png, jpg, bmp, tga, ppm are supported. File format is determined by the given extension.
        bool snapshot_color(unsigned int index, const std::string& file_name) const;

        /// Snapshots the color render buffer into an image file.
        /// \note Only png, jpg, bmp, tga, ppm are supported. File format is determined by the given extension.
        bool snapshot_depth(const std::string& file_name) const;

        /**
         * \brief Blit the whole sized buffer
         *
         * \details Blits from the source rectangle in the source framebuffer object to the target rectangle in the
         *      target framebuffer object. If source or target is 0, the default framebuffer will be used instead
         *      of a framebuffer object as source or target respectively. The buffers parameter should be a mask
         *      consisting of any combination of GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, and GL_STENCIL_BUFFER_BIT.
         *      Any buffer type that is not present both in the source and target buffers is ignored.
         *
         *      The source and target rectangles may have different sizes; in this case buffers should not contain
         *      GL_DEPTH_BUFFER_BIT or GL_STENCIL_BUFFER_BIT. The filter parameter should be set to GL_LINEAR or
         *      GL_NEAREST specifying whether linear or nearest interpolation should be used for scaling.
         *      For GL_DEPTH_BUFFER_BIT or GL_STENCIL_BUFFER_BIT, the filter parameter must be GL_NEAREST.
         *
         *      If source equals target a copy is performed within the same buffer. Results are undefined if the
         *      source and target rectangles overlap and have different sizes. The sizes must also be the same
         *      if any of the framebuffer objects are multisample framebuffers.
         *
         *      When multiple render targets are in use, source_color_attachment_index and target_color_attachment_index
         *      specify the index of the color attachments in the source and destination framebuffers.
         *
         * \note The scissor test will restrict the blit area if enabled.
         */
         static void blit_framebuffer(
             FramebufferObject* target, const FramebufferObject* source,
             GLbitfield buffers,	// e.g., GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT, or GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
             GLenum filter = GL_NEAREST
         );

        /// Blit the whole sized buffer of a specific color attachment.
        static void blit_framebuffer(
            FramebufferObject* target, const FramebufferObject* source,
            int target_color_attachment_index, int source_color_attachment_index,
            GLbitfield buffers,	// e.g., GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT, or GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
            GLenum filter = GL_NEAREST
        );

        /// Blit a specified region
        static void blit_framebuffer(
            FramebufferObject* target, int tx0, int ty0, int tx1, int ty1,
            const FramebufferObject* source, int sx0, int sy0, int sx1, int sy1,
            GLbitfield buffers,	// e.g., GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT, or GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
            GLenum filter = GL_NEAREST
        );

        /// Blit a specified region of a specific color attachment.
        static void blit_framebuffer(
            FramebufferObject* target, int tx0, int ty0, int tx1, int ty1,
            const FramebufferObject* source, int sx0, int sy0, int sx1, int sy1,
            int target_color_attachment_index, int source_color_attachment_index,
            GLbitfield buffers,	// e.g., GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT, or GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
            GLenum filter = GL_NEAREST
        );

    private:

        void init(int w, int h, int samples);

        void clear();

        bool check_status() const;

        // Display a given attachment for the current framebuffer object.
        void _print_attachment(unsigned int index) const;

        // Display any buffer (convert value into string).
        void _print_buffer(int value) const;

        void _prepare_resolve_fbo();

    private:
        GLuint fbo_id_;
        GLuint prev_draw_fbo_;		// for release()
        GLuint prev_read_fbo_;		// for release()

        int width_;
        int height_;

        int samples_;
        FramebufferObject* resolved_fbo_;	// for multisample FBO,

        GLenum texture_target_;

        GLuint depth_buffer_;
        GLuint depth_texture_;
        GLenum depth_internal_format_;
        GLenum depth_texture_filter_;
        GLenum depth_texture_compare_mode_;
        GLenum depth_texture_compare_func_;

        struct ColorAttachment {
            ColorAttachment() : buffer(0), texture(0), internal_format(GL_RGBA8), format(GL_RGBA), type(GL_UNSIGNED_BYTE), texture_filter(GL_NEAREST) {}
            GLuint buffer;
            GLuint texture;
            GLenum internal_format;
            GLenum format;
            GLenum type;
            GLenum texture_filter;
        };
        std::vector<ColorAttachment> color_attachments_;

        bool   valid_;

    private:
        //copying disabled
        FramebufferObject(const FramebufferObject&);
        FramebufferObject& operator=(const FramebufferObject&);
    };


} // namespace MV


#endif // EASY3D_RENDERER_FRAMEBUFFER_OBJECT_H
