
import scipy.io as scio
import os
import numpy as np
import vtk
from vtk.util.numpy_support import vtk_to_numpy, numpy_to_vtk
from vtk.util.vtkConstants import *



model = 'Male'
full_path = 'F:\\UniformMF\\Human_models\\'+model +'_Seat'
inc = 1


def numpyarray2vtkimagedata(arr, space=[1.0, 1.0, 1.0], origin=[0, 0, 0]):
    vtk_data = numpy_to_vtk(
        arr.ravel(), array_type=vtk.VTK_UNSIGNED_CHAR)
    img = vtk.vtkImageData()
    shape = arr.shape
    img.SetDimensions(shape[2], shape[1], shape[0])
    img.SetSpacing(space)
    img.SetOrigin(origin)
    img.GetPointData().SetScalars(vtk_data)
    return img


def generate_surface(images):
    image3D = np.where(images > 0, 100, 0)
    img = numpyarray2vtkimagedata(image3D.astype('uint8'))
    Extractor = vtk.vtkContourFilter()
    Extractor.SetInputData(img)
    Extractor.SetValue(0, 100)
    Extractor.Update()

    hole_fill = vtk.vtkFillHolesFilter()
    hole_fill.SetInputData(Extractor.GetOutput())
    hole_fill.Update()

    smoother = vtk.vtkSmoothPolyDataFilter()
    smoother.SetInputData(hole_fill.GetOutput())
    smoother.SetNumberOfIterations(36)
    smoother.BoundarySmoothingOn()
    smoother.FeatureEdgeSmoothingOn()
    smoother.SetFeatureAngle(30.0)
    smoother.Update()

    Normals = vtk.vtkPolyDataNormals()
    Normals.SetInputData(smoother.GetOutput())
    Normals.SetFeatureAngle(60.0)
    Normals.Update()

    Stripper = vtk.vtkStripper()
    Stripper.SetInputData(Normals.GetOutput())
    Stripper.Update()


    return Stripper.GetOutput()

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print('{}:  Spatial increment is {} mm'.format(model, inc))
    ori_surface_file = os.path.join(full_path, 'orignal_'+model+'.stl')
    image_file = os.path.join(full_path, model+'.mat')
    dat = scio.loadmat(image_file)
    image3D = dat['image']
    image3D = np.where(image3D == 96, 0, image3D)

    pd = generate_surface(image3D)
    writer = vtk.vtkSTLWriter()
    writer.SetFileTypeToASCII()
    writer.SetFileName(ori_surface_file)
    writer.SetInputData(pd)
    writer.Write()
    print('all data processed ...')