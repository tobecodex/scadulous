import trimesh

class Scene:
  def __init__(self, name="scene"):
    self._meshes = []

  def add(self, mesh):
    self._objects.add(mesh)

  def save(self):
    pass

class Mesh:
  def __init__(self, m = None):
    self._mesh = m

  def load(self, path):
    self._mesh = trimesh.load_mesh(path)

  def export(self):
    self._mesh.export("mesh.stl")

  def stl(self):
    return trimesh.exchange.stl.export_stl_ascii(self._mesh)

  def split(self, p, n):
    plane, transform = self._mesh.section(n, p).to_planar()
    v,f = plane.triangulate()
    v3 = [] 
    for v2 in v:
      v3.append([v2[0], v2[1], 0])
    cap = trimesh.Trimesh(v3, f).apply_transform(transform)
    np = [-n[0], -n[1], -n[2]]
    return (
      Mesh(cap.union(self._mesh.slice_plane(p, n))),
      Mesh(cap.union(self._mesh.slice_plane(p, np)))
    )      

  def transform(self, t):
    self._mesh.apply_transform(t)

  def translate(self, t):
    t = trimesh.transformations.translation_matrix(t)
    self.transform(t)

  def rotate(self, r):
    r = trimesh.transformations.rotation_matrix(r)
    self.transform(r)

  def scale(self, s):
    s = trimesh.transformations.scale_matrix(s, [0,0,0])
    self.transform(s) 

  def join(self, other):
    return Mesh(self._mesh.union(other._mesh))

  def show():
    self._mesh.show()

def send(stl_ascii):
  import socket

  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  s.connect(("172.29.32.1", 4242))
  s.send(stl_ascii.encode())

m = Mesh()
m.load("viewers/test_files/teapot.stl")
#box = Mesh(trimesh.creation.box((1, 1, 1)))
#m = box.join(m)
#axis = Mesh(trimesh.creation.axis())
#box = box.split([0,0,0], [0,1,0])
#box[0].translate([0, 1, 0])
#box[1].scale(1.5)
#box = box[0].join(box[1])
#m = m.join(axis)
send(m.stl())
