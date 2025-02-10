#include "widget_checker_sphere.h"

#include <QMouseEvent>
#include <QOpenGLFunctions>

#include "../../renderer/camera.h"
#include "../../renderer/drawable_triangles.h"
#include "../../renderer/shader_program.h"
#include "../../renderer/shader_manager.h"
#include "../../renderer/opengl_util.h"
#include "../../renderer/shape.h"
#include "../../util/setting.h"

using namespace MV;

WidgetCheckerSphere::WidgetCheckerSphere(QWidget* parent) : QOpenGLWidget(parent)
{
    m_pCamera = nullptr;
    m_pCheckerSphere = nullptr;
    m_pLightSphere = nullptr;
    m_bMouseDown = false;
    m_fSceneRadius = 1.1f;

    m_vecDefaultLightPos = setting::light_position;
    m_vecDefaultLightPos = normalize(m_vecDefaultLightPos);
    m_vecLightPos = m_vecDefaultLightPos;

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

WidgetCheckerSphere::~WidgetCheckerSphere()
{
    makeCurrent();
    delete m_pCheckerSphere;
    delete m_pLightSphere;
    delete m_pCamera;
    doneCurrent();
}

void WidgetCheckerSphere::initializeGL()
{
    QOpenGLWidget::initializeGL();

    QOpenGLFunctions* func = context()->functions();
    func->initializeOpenGLFunctions();
    OpenglUtil::init();

    func->glEnable(GL_DEPTH_TEST);
    func->glClearDepthf(1.0f);
    func->glClearColor(
        static_cast<float>(m_BackgroundColor.redF()),
        static_cast<float>(m_BackgroundColor.greenF()),
        static_cast<float>(m_BackgroundColor.blueF()),
        1.0f
    );

    m_pCamera = new Camera;
    m_pCamera->setType(Camera::ORTHOGRAPHIC);
    m_pCamera->setScreenWidthAndHeight(width(), height());
    m_pCamera->setPosition(vec3(0.0f, 0.0f, 1.0f));
    // either set view direction
    //camera_->setViewDirection(vec3(0.0, 0.0, -1.0));
    // or set lookAt point
    m_pCamera->lookAt(vec3(0.0f, 0.0f, 0.0f));
    // allow some margin for the light sphere to be visible near the border of the viewer
    m_pCamera->setSceneRadius(1.1f);
    m_pCamera->showEntireScene();
}


void WidgetCheckerSphere::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);

    // The viewport is set up by QOpenGLWidget before drawing.
    // So I don't need to set up.
    // glViewport(0, 0, static_cast<int>(w * highdpi_), static_cast<int>(h * highdpi_));

    m_pCamera->setScreenWidthAndHeight(w, h);
}


void WidgetCheckerSphere::mousePressEvent(QMouseEvent* e)
{
    m_bMouseDown = true;
    const vec3& p = ProjectToSphere(e->pos());

    // set for light position hint on the sphere
    m_vecLightPos = vec4(p.x, p.y, p.z, 0);
    UpdateLighting();
}


void WidgetCheckerSphere::mouseMoveEvent(QMouseEvent* e)
{
    if (m_bMouseDown)
    {
        const vec3& p = ProjectToSphere(e->pos());

        // set for light position hint on the sphere
        m_vecLightPos = vec4(p.x, p.y, p.z, 0);
        UpdateLighting();
    }
}


void WidgetCheckerSphere::mouseReleaseEvent(QMouseEvent* e)
{
    m_bMouseDown = false;
}

void WidgetCheckerSphere::UpdateLighting()
{
    setting::light_position = m_vecLightPos;
    update();
    emit eLightPositionChanged();
}

void WidgetCheckerSphere::CreateSpheres()
{
    const vec3 center(0, 0, 0);
    const double radius = 1.0;
    const int checker_size = 5;
    const int slices = 50;
    const int stacks = 40;
    const vec3 color1(0.6f, 0.6f, 0.6f);
    const vec3 color2(1, 1, 1);

    std::vector<vec3> vertices, normals, colors;
    shape::create_checker_sphere(center, radius, slices, stacks, checker_size, color1, color2, vertices, normals, colors);

    m_pCheckerSphere = new TrianglesDrawable("checker_sphere");
    m_pCheckerSphere->update_vertex_buffer(vertices);
    m_pCheckerSphere->update_normal_buffer(normals);
    m_pCheckerSphere->update_color_buffer(colors);

    // light
    vertices.clear();
    normals.clear();
    colors.clear();
    shape::create_sphere(vec3(0, 0, 0), 0.1, 10, 10, vec3(1, 0, 0), vertices, normals, colors);
    m_pLightSphere = new TrianglesDrawable("light");
    m_pLightSphere->update_vertex_buffer(vertices);
    m_pLightSphere->update_normal_buffer(normals);
    m_pLightSphere->update_color_buffer(colors);
}


MV::vec3 WidgetCheckerSphere::ProjectToSphere(const QPoint& p)
{
    float x = p.x() * (m_fSceneRadius * 2.0f / width()) - m_fSceneRadius;
    float y = m_fSceneRadius - p.y() * (m_fSceneRadius * 2.0f / height());
    float z = 0.0f;
    float sqr_len = x * x + y * y;
    if (sqr_len > 1)
    {
        // if the projection point is not over the sphere, then normalize it
        float norm = 1.0f / std::sqrt(sqr_len);
        x *= norm;
        y *= norm;
        z = 0.0f;
    }
    else
    { // computing the z coordinate of projected point
        z = std::sqrt(1.0f - sqr_len);
    }
    return vec3(x, y, z);
}

void WidgetCheckerSphere::rResetLightPosition() 
{
    // set for light position hint on the sphere
    m_vecLightPos = m_vecDefaultLightPos;
    UpdateLighting();
}

void WidgetCheckerSphere::paintGL()
{
    if (!m_pCheckerSphere)
    {
        CreateSpheres();
    }

    ShaderProgram* program = ShaderManager::get_program("surface/surface");
    if (!program)
    {
        std::vector<ShaderProgram::Attribute> attributes;
        attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
        attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::TEXCOORD, "vtx_texcoord"));
        attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::COLOR, "vtx_color"));
        attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::NORMAL, "vtx_normal"));
        program = ShaderManager::create_program_from_files("surface/surface", attributes);
    }
    if (!program)
    {
        return;
    }

    const mat4& MVP = m_pCamera->modelViewProjectionMatrix();
    // camera position is defined in world coordinate system.
    const vec3& wCamPos = m_pCamera->position();
    // it can also be computed as follows:
    //const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
    const mat4& MV = m_pCamera->modelViewMatrix();
    const vec4& wLightPos = inverse(MV) * setting::light_position;

    vec4 xxxx = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    program->bind();
    program->set_uniform("MVP", MVP)
        ->set_uniform("NORMAL", mat3::identity())   // needs be padded when using uniform blocks
        ->set_uniform("lighting", true)
        ->set_uniform("wLightPos", wLightPos)
        ->set_uniform("wCamPos", wCamPos)
        ->set_uniform("two_sides_lighting", false)
        ->set_uniform("distinct_back_color", false)
        ->set_uniform("smooth_shading", true)
        ->set_uniform("ssaoEnabled", false)
        ->set_uniform("per_vertex_color", true)
        ->set_block_uniform("Material", "ambient", xxxx)
        ->set_block_uniform("Material", "specular", setting::material_specular)
        ->set_block_uniform("Material", "shininess", &setting::material_shininess)
        ->set_uniform("highlight", false)
        ->set_uniform("selected", false)
        ->set_uniform("highlight_color", setting::highlight_color)
        ->set_uniform("clippingPlaneEnabled", false)
        ->set_uniform("use_texture", false);

    program->set_uniform("MANIP", mat4::translation(m_vecLightPos.xyz()));
    m_pLightSphere->gl_draw();

    program->set_uniform("MANIP", mat4::identity());
    m_pCheckerSphere->gl_draw();

    program->release();
}
