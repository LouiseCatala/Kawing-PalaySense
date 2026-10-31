from pathlib import Path
import trimesh,numpy as np,vtk
from vtk.util.numpy_support import numpy_to_vtk,numpy_to_vtkIdTypeArray
from PIL import Image,ImageDraw,ImageFont
D=Path(__file__).resolve().parent
scene=trimesh.load(str(D/'KAWING_PalaySense_No_Extra_RC.glb'),force='scene')
def render(outfile,target,loc,scale,caption):
 renderer=vtk.vtkRenderer();renderer.SetBackground(.055,.077,.088);renderer.SetBackground2(.14,.17,.19);renderer.GradientBackgroundOn()
 for mesh in scene.geometry.values():
  v=np.asarray(mesh.vertices,dtype=np.float64);f=np.asarray(mesh.faces,dtype=np.int64)
  pts=vtk.vtkPoints();pts.SetData(numpy_to_vtk(v,deep=True))
  cell=vtk.vtkCellArray();cell.SetCells(len(f),numpy_to_vtkIdTypeArray(np.column_stack((np.full(len(f),3),f)).astype(np.int64).ravel(),deep=True))
  poly=vtk.vtkPolyData();poly.SetPoints(pts);poly.SetPolys(cell)
  mapper=vtk.vtkPolyDataMapper();mapper.SetInputData(poly)
  actor=vtk.vtkActor();actor.SetMapper(mapper)
  co=np.asarray(mesh.visual.face_colors[0][:3],float)/255
  actor.GetProperty().SetColor(*co);actor.GetProperty().SetAmbient(.45);actor.GetProperty().SetDiffuse(.59);actor.GetProperty().SetSpecular(.20);actor.GetProperty().SetSpecularPower(24)
  renderer.AddActor(actor)
 w=vtk.vtkRenderWindow();w.SetOffScreenRendering(1);w.SetMultiSamples(3);w.AddRenderer(renderer);w.SetSize(1550,1020)
 camera=renderer.GetActiveCamera();camera.SetFocalPoint(*target);camera.SetPosition(*loc);camera.SetViewUp(0,1,0);camera.ParallelProjectionOn();camera.SetParallelScale(scale);renderer.ResetCameraClippingRange()
 for pos,intensity in [((45,80,180),.9),((-100,-75,100),.45)]:
  light=vtk.vtkLight();light.SetLightTypeToSceneLight();light.SetPosition(*pos);light.SetFocalPoint(60,-50,0);light.SetIntensity(intensity);renderer.AddLight(light)
 w.Render();filt=vtk.vtkWindowToImageFilter();filt.SetInput(w);filt.Update();wr=vtk.vtkPNGWriter();wr.SetFileName(str(D/outfile));wr.SetInputConnection(filt.GetOutputPort());wr.Write()
 im=Image.open(D/outfile).convert('RGBA');dr=ImageDraw.Draw(im,'RGBA');bold='/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf';normal='/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf'
 dr.rounded_rectangle((35,27,1300,150),radius=17,fill=(5,22,31,210),outline=(65,208,174,125),width=2)
 dr.text((60,39),'KAWING  /  PALAYSENSE',fill=(242,251,246),font=ImageFont.truetype(bold,49))
 dr.text((62,108),caption,fill=(145,224,213),font=ImageFont.truetype(normal,22))
 im.convert('RGB').save(D/outfile)
 print('RENDERED',outfile,flush=True)
render('KAWING_No_Extra_RC_Isometric.png',(63,-45,0),(182,-175,220),76,'ESP32 + TP4056 + MT3608 + SW1  |  NO EXTERNAL R OR C')
render('KAWING_No_Extra_RC_Top.png',(64,-48,0),(64,-48,240),66,'BOARD-MOUNTED MODULES  |  CLEAN 6-FOOTPRINT REVISION')
