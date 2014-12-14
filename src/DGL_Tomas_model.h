//C++

/*
----------------------------------------------------
The Desktop Project
------------------

Copyright 2004 Daher Alfawares

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

----------------------------------------------------
*/

#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <fstream>
#include <vector>
#include <map>
#include <string>

namespace DGL {
	namespace Tomas {


/**
 * Base class for loading and drawing objects.
 * This should (must) be inherited by a class that does the real loading.
 * The last thing that should be done after loading an object, is to call
 * CreateDisplayLists();
 */
class Model {
 public:

  void Init();
  void Destroy();

  /**
   * Render the model.
   */
  inline void Render() {
    glCallList(dlist_);
    if(showbound_) {
      glPushAttrib(GL_LIGHTING_BIT|GL_TEXTURE_BIT);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);
      glBegin(GL_TRIANGLE_STRIP);
      for(unsigned int i=0;i<boundcoords_.size();i++)
	glVertex3f(boundcoords_[i].x_(),boundcoords_[i].y_(),
		   boundcoords_[i].z_());
      glEnd();
      glPopAttrib();
    }
  }

  /**
   * Show the bounding box (if one has been set).
   * @param show is true iff the box should be shown.
   */
  void ShowBoundingBox(bool show);

  /**
   * Destructor. Duh!
   */
  ~Model();
 protected:
  /* Vertex class.
   * Holds both vertex coordinates and a list of faces that the vertex is
   * used in.
   */
  class Vertex {
   public:
    Vertex(Vector v) : coords_(v) {}
    Vector coords_;
    std::vector<GLushort> usedby_;    
  };

  /**
   * Triangle class.
   * Holds material, normal, vertices and smoothing groups.
   */
  class Triangle {
   public:
    GLushort v1_,v2_,v3_;
    GLuint groups_;
    GLint material_;
    Vector normal_;
    Triangle(GLushort *v);
  };

  /**
   * Material class.
   * Holds properties such as color, shininess, texture...
   */
  class Material {
   public:
	   DGL::Texture texid_;
    enum Parameter {None,Ambient,Diffuse,Specular,Shininess,Texture,
		    Transparency};
    enum TextureMode {TextureModulate,TextureReplace,TextureDecal,
		      TextureBlend};

    /**
     * Constructor...
     */
    Material();

    void SetTextureMode(TextureMode flag);

    /**
     * Set the type of parameter we want to change.
     * For example Ambient, Texture... This is used by the Set calls.
     * @param param is the parameter type
     */
    void SetParameter(Parameter param);

    /**
     * Set a parameter.
     * Sets the parameter selected with SetParameter.
     * @param values is an array containing the values.
     */
    void Set(const GLfloat *values);

    /**
     * Set a parameter.
     * Sets the parameter selected with SetParameter.
     * @param values is an array containing the values.
     */
    void Set(const GLubyte *values);

    /**
     * Set a (file) name.
     * Sets the name for the parameter selected with SetParameter.
     * @param name is the name.
     */
    void Set(char *name);
    void Set(const GLfloat value);
    void Set(const GLushort value);

    /**
     * Set alpha value.
     * @param alpha is the value in the range [0..1]
     */
    void SetAlpha(const GLfloat alpha);

    /**
     * Set ambient color.
     * @param ambient is an array of three values (r,g,b) in the range [0..1].
     */
    void SetAmbient(const GLfloat *ambient);

    /**
     * Set diffuse color.
     * @param diffuse is an array of three values (r,g,b) in the range [0..1].
     */
    void SetDiffuse(const GLfloat *diffuse);

    /**
     * Set specular color.
     * @param specular is an array of three values (r,g,b) in the range [0..1].
     */
    void SetSpecular(const GLfloat *specular);

    /**
     * Set shininess.
     * @param shininess is a value in the range [0..1].
     */
    void SetShininess(const GLfloat shininess);

    void SetTwoSided(const bool twosided);
    void Select();
   private:
    bool twosided_;
    GLfloat ambient_[4],diffuse_[4],specular_[4],shininess_,alpha_;
    Parameter param_;
    int texmode_;
  };

  /**
   * Mesh class.
   * Holds list of vertices, texture coordinates, triangles, materials...
   */
  class Mesh {
    Matrix matrix_;
    std::vector<Vertex> v_;
    std::vector<Vector> texv_;
    std::vector<Triangle> tri_;
    std::string name_;

    void CalculateNormal(const GLushort face);
   public:
    Mesh(const char *name);
    ~Mesh();
    /**
     * Set the transformation of the mesh.
     * @param mat is the matrix;
     */
    void SetTransform(const Matrix &mat);

    /**
     * Add a vertex
     * @param vec is the vertex to add.
     */
    void AddVertex(const Vector &vec);

    /**
     * Add a texture coordinate.
     * @param vec is the coordinate to add. The third coordinate is ignored.
     */
    void AddTexCoord(const Vector &vec);

    /**
     * Add a triangle.
     * @param tri is the triangle to add.
     */
    void AddTriangle(const Triangle &tri);

    /**
     * Set the material for a face.
     * @param face is the face ID (the first face added has ID 0, the next 1..)
     * @param mat is the material ID (use GetMaterial(name) to get it).
     */
    void SetMaterial(const GLushort face,const GLint mat);

    /**
     * Set the smoothing groups for a face.
     * @param face is the face ID (see SetMaterial)
     * @param groups is 32 bits. Bit n means the face is part of group n.
     */
    void SetSmoothGroups(const GLushort face,const GLuint groups);

    /**
     * Draw the face.
     * You probably won't be using this :)
     */
    void Draw();

    /**
     * Get number of faces.
     * @return the number of faces (triangles) in the mesh.
     */
    unsigned int NFaces();

    /**
     * Get the number of vertices.
     * @return the number of vertices in the mesh.
     */
    unsigned int NVertices();
  };

  /**
   * Create display lists of all the meshes.
   */
  void CreateDisplayLists();

  /**
   * Constructor...
   */
  Model();

  /**
   * Get the current mesh.
   * @return the current mesh.
   */
  Mesh &GetCurrentMesh();

  /**
   * Get the current number of meshes.
   * @return number of meshes
   */
  unsigned int GetNMeshes();

  /**
   * Add a mesh.
   * Actually it's more like pushing an empty mesh onto a stack. This can then
   * be modified by getting it with GetCurrentMesh() and using the functions in
   * the Mesh class.
   * @param name is the name of the mesh.
   */
  void AddMesh(const char *name);

  /**
   * Get the current material.
   * @return the current material.
   */
  Material &GetCurrentMaterial();

  /**
   * Add a material.
   * See comments on AddMesh().
   * @param name is the name of the material.
   */
  void AddMaterial(const char *name);

  /**
   * Get a materialID.
   * @param name is the name of the material.
   * @return the ID.
   */
  int GetMaterialID(const char *name);

  /**
   * Set a bounding box.
   * @param minx is the minimum x.
   * @param miny is the minimum y.
   * @param minz is the minimum z.
   * @param maxx is the maximum x.
   * @param maxy is the maximum y.
   * @param maxz is the maximum z.
   */
  void SetBoundingBox(float minx,float miny,float minz,
		      float maxx,float maxy,float maxz);

  // file for logging
  std::ofstream log;
 private:
	 std::vector<DGL::Texture> textures_;
  bool showbound_;
  std::vector<Vector> boundcoords_;
  static std::vector<Mesh> meshes_;
  static std::map<std::string,unsigned int> materialnames_;
  static std::vector<Material> materials_;
  GLuint dlist_;
};

	}// namespace Tomas
}// namespace DGL

#endif
