#include <stdlib.h>
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include "file-util.h"
#include "gl-util.h"
#include "meshes.h"

static struct {
    struct flag_mesh flag, background;
    struct flag_vertex *flag_vertex_array;
    
    struct {
        GLuint vertex_shader, fragment_shader, program;

        struct {
            GLint texture, p_matrix;
        } uniforms;

        struct {
            GLint position, normal, texcoord;
        } attributes;
    } flag_program;

    GLfloat p_matrix[16];
} g_resources;

static void init_gl_state(void)
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

#define PROJECTION_FOV_RATIO 0.7f
#define PROJECTION_NEAR_PLANE 0.0625f
#define PROJECTION_FAR_PLANE 256.0f

static void init_p_matrix(GLfloat *matrix, int w, int h)
{
    GLfloat wf = (GLfloat)w, hf = (GLfloat)h;
    GLfloat
        r_xy_factor = fminf(wf, hf) * 1.0f/PROJECTION_FOV_RATIO,
        r_x = r_xy_factor/wf,
        r_y = r_xy_factor/hf,
        r_zw_factor = 1.0f/(PROJECTION_FAR_PLANE - PROJECTION_NEAR_PLANE),
        r_z = (PROJECTION_NEAR_PLANE + PROJECTION_FAR_PLANE)*r_zw_factor,
        r_w = -2.0f*PROJECTION_NEAR_PLANE*PROJECTION_FAR_PLANE*r_zw_factor;

    matrix[ 0] = r_x;  matrix[ 1] = 0.0f; matrix[ 2] = 0.0f; matrix[ 3] = 0.0f;
    matrix[ 4] = 0.0f; matrix[ 5] = r_y;  matrix[ 6] = 0.0f; matrix[ 7] = 0.0f;
    matrix[ 8] = 0.0f; matrix[ 9] = 0.0f; matrix[10] = r_z;  matrix[11] = 1.0f;
    matrix[12] = 0.0f; matrix[13] = 0.0f; matrix[14] = r_w;  matrix[15] = 0.0f;

    // XXX apply translation
}

void render_mesh(struct flag_mesh const *mesh)
{
    glBindTexture(GL_TEXTURE_2D, mesh->texture);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
    glVertexAttribPointer(
        g_resources.flag_program.attributes.position,
        3, GL_FLOAT, GL_FALSE, sizeof(struct flag_vertex),
        (void*)offsetof(struct flag_vertex, position)
    );
    glVertexAttribPointer(
        g_resources.flag_program.attributes.normal,
        3, GL_FLOAT, GL_FALSE, sizeof(struct flag_vertex),
        (void*)offsetof(struct flag_vertex, normal)
    );
    glVertexAttribPointer(
        g_resources.flag_program.attributes.texcoord,
        2, GL_FLOAT, GL_FALSE, sizeof(struct flag_vertex),
        (void*)offsetof(struct flag_vertex, texcoord)
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->element_buffer);
    glDrawElements(GL_TRIANGLES, mesh->element_count, GL_UNSIGNED_SHORT, (void*)0);
}

static int make_resources(void)
{
    g_resources.flag_vertex_array = init_flag_mesh(&g_resources.flag);
    init_background_mesh(&g_resources.background);

    g_resources.flag.texture = make_texture("flag.tga");
    g_resources.background.texture = make_texture("background.tga");

    g_resources.flag_program.vertex_shader
        = make_shader(GL_VERTEX_SHADER, "flag.v.glsl");
    g_resources.flag_program.fragment_shader
        = make_shader(GL_FRAGMENT_SHADER, "flag.f.glsl");

    g_resources.flag_program.program = make_program(
        g_resources.flag_program.vertex_shader,
        g_resources.flag_program.fragment_shader
    );

    // XXX error checking

    init_p_matrix(g_resources.p_matrix, 640, 480);

    g_resources.flag_program.uniforms.texture
        = glGetUniformLocation(g_resources.flag_program.program, "texture");
    g_resources.flag_program.uniforms.p_matrix
        = glGetUniformLocation(g_resources.flag_program.program, "p_matrix");

    g_resources.flag_program.attributes.position
        = glGetAttribLocation(g_resources.flag_program.program, "position");
    g_resources.flag_program.attributes.normal
        = glGetAttribLocation(g_resources.flag_program.program, "normal");
    g_resources.flag_program.attributes.texcoord
        = glGetAttribLocation(g_resources.flag_program.program, "texcoord");

    return 1;
}

static void update(void)
{
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);
    GLfloat seconds = (GLfloat)milliseconds * (1.0f/1000.0f);

    update_flag_mesh(&g_resources.flag, g_resources.flag_vertex_array, seconds);
    glutPostRedisplay();
}

static void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    glUseProgram(g_resources.flag_program.program);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(g_resources.flag_program.uniforms.texture, 0);

    glUniformMatrix4fv(
        g_resources.flag_program.uniforms.p_matrix,
        1, GL_FALSE,
        g_resources.p_matrix
    );

    glEnableVertexAttribArray(g_resources.flag_program.attributes.position);
    glEnableVertexAttribArray(g_resources.flag_program.attributes.normal);
    glEnableVertexAttribArray(g_resources.flag_program.attributes.texcoord);

    render_mesh(&g_resources.flag);
    render_mesh(&g_resources.background);

    glDisableVertexAttribArray(g_resources.flag_program.attributes.position);
    glDisableVertexAttribArray(g_resources.flag_program.attributes.normal);
    glDisableVertexAttribArray(g_resources.flag_program.attributes.texcoord);
    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Flag");
    glutIdleFunc(&update);
    glutDisplayFunc(&render);

    glewInit();
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "OpenGL 2.0 not available\n");
        return 1;
    }

    init_gl_state();
    if (!make_resources()) {
        fprintf(stderr, "Failed to load resources\n");
        return 1;
    }

    glutMainLoop();
    return 0;
}
