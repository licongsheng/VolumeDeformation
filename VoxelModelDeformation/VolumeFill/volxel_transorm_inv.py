
import scipy.io as scio


# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import os
import numpy as np
import vtk
from vtk.util.numpy_support import vtk_to_numpy, numpy_to_vtk
from vtk.util.vtkConstants import *
import scipy.io as scio
import matplotlib.pyplot as plt
from laplace_deformation import *
from tqdm import tqdm

color_list = plt.cm.tab20(np.linspace(0, 1, 50))


model = 'Leg'
full_path = 'F:\\UniformMF\\Human_models\\'+model+'_refinement'
inc = 2

def render_polydata(pd):
    cleanfilter = vtk.vtkCleanPolyData()
    cleanfilter.SetInputData(pd)
    cleanfilter.Update()
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(cleanfilter.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)

    ren = vtk.vtkRenderer()
    ren.AddActor(actor)
    ren.SetBackground(0.1, 0.2, 0.4)
    ren.ResetCamera()
    ren.GetActiveCamera().Zoom(1)

    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetSize(200, 200)
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)
    iren.Initialize()
    iren.Start()


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


def polydata2actor(pd, color=[200, 100, 0]):
    cleanfilter = vtk.vtkCleanPolyData()
    cleanfilter.SetInputData(pd)
    cleanfilter.Update()
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(cleanfilter.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(color)
    return actor


def volumeRender_1(image3D):
    img = numpyarray2vtkimagedata(image3D.astype('uint8'))
    #Create transfer mapping scalar value to opacity
    opacityTransferFunction = vtk.vtkPiecewiseFunction()
    opacityTransferFunction.AddPoint(0, 0.0)
    opacityTransferFunction.AddPoint(1, 1.0)
    opacityTransferFunction.AddPoint(255, 1.0)

    colorTransferFunction = vtk.vtkColorTransferFunction()
    colorTransferFunction.AddRGBPoint(0.0,0.0,0.0,0.0)
    colorTransferFunction.AddRGBPoint(64.0,1.0,0.0,1.0)
    colorTransferFunction.AddRGBPoint(128.0,0.0,0.0,0.0)
    colorTransferFunction.AddRGBPoint(192.0,0.0,1.0,0.0)
    colorTransferFunction.AddRGBPoint(255.0,0.0,0.2,0.0)

    volumeProperty = vtk.vtkVolumeProperty()
    volumeProperty.SetColor(colorTransferFunction)
    volumeProperty.SetScalarOpacity(opacityTransferFunction)
    volumeProperty.ShadeOn()
    volumeProperty.SetInterpolationTypeToLinear()

    volumeMapper = vtk.vtkSmartVolumeMapper()
    volumeMapper.SetInputData(img)
    #volumeMapper.SetVolumeRayCastFunction(compositeFunction)

    volume = vtk.vtkVolume()
    volume.SetMapper(volumeMapper)
    volume.SetProperty(volumeProperty)
    ren = vtk.vtkRenderer()

    ren.AddVolume(volume)

    axesActor = vtk.vtkAxesActor()
    axesActor.SetTotalLength(400,400,400)
    ren.AddActor(axesActor)


    ren.SetBackground(0.1, 0.2, 0.4)
    ren.ResetCamera()
    ren.GetActiveCamera().Zoom(1)

    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetSize(200, 200)
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)
    iren.Initialize()
    iren.Start()


def get_min_extent_images(image):
    shape = image.shape
    shift_x = -1
    shift_y = -1
    shift_z = -1
    found = False
    for i in range(shape[0]):
        slice = image[i,:,:]
        if found is False and np.sum(slice) > 0:
            shift_x = i
            found = True
    found = False
    for i in range(shape[1]):
        slice = image[:,i,:]
        if found is False and np.sum(slice) > 0:
            shift_y = i
            found = True
    found = False
    for i in range(shape[2]):
        slice = image[:,:, i]
        if found is False and np.sum(slice) > 0:
            shift_z = i
            found = True

    return shift_x, shift_y, shift_z


def polydata_transform(pd, scale=1.0):
    transform = vtk.vtkTransform()
    filter = vtk.vtkTransformPolyDataFilter()
    filter.SetInputData(pd)
    transform.Scale(scale,scale,scale)
    filter.SetTransform(transform)
    filter.Update()
    pd = filter.GetOutput()
    #bounds = pd.GetBounds()
    #print(bounds)
    return pd


def polydate_translate(pd, translate=[0,0,0]):
    transform = vtk.vtkTransform()
    filter = vtk.vtkTransformPolyDataFilter()
    filter.SetInputData(pd)
    transform.Translate(translate[0], translate[1], translate[2])
    #transform.Scale(0.96, 0.96, 0.96)
    filter.SetTransform(transform)
    filter.Update()

    pd = filter.GetOutput()
    bounds = pd.GetBounds()
    #print(bounds)
    return pd


def create_box_actor(bound, color=None):
    minX, maxX, minY, maxY, minZ, maxZ = bound
    boxGridPoints = vtk.vtkPoints()
    boxGridPoints.SetNumberOfPoints(8)
    boxGridPoints.SetPoint(0, minX, maxY, minZ)
    boxGridPoints.SetPoint(1, maxX, maxY, minZ)
    boxGridPoints.SetPoint(2, maxX, minY, minZ)
    boxGridPoints.SetPoint(3, minX, minY, minZ)

    boxGridPoints.SetPoint(4, minX, maxY, maxZ)
    boxGridPoints.SetPoint(5, maxX, maxY, maxZ)
    boxGridPoints.SetPoint(6, maxX, minY, maxZ)
    boxGridPoints.SetPoint(7, minX, minY, maxZ)

    boxGridCellArray = vtk.vtkCellArray()
    for i in range(12):
        boxGridCell = vtk.vtkLine()
        if i < 4:
            temp_data = (i + 1) if (i + 1) % 4 != 0 else 0
            boxGridCell.GetPointIds().SetId(0, i)
            boxGridCell.GetPointIds().SetId(1, temp_data)
        elif i < 8:
            temp_data = (i + 1) if (i + 1) % 8 != 0 else 4
            boxGridCell.GetPointIds().SetId(0, i)
            boxGridCell.GetPointIds().SetId(1, temp_data)
        else:
            boxGridCell.GetPointIds().SetId(0, i % 4)
            boxGridCell.GetPointIds().SetId(1, i % 4 + 4)
        boxGridCellArray.InsertNextCell(boxGridCell)

    boxGridData = vtk.vtkPolyData()
    boxGridData.SetPoints(boxGridPoints)
    boxGridData.SetLines(boxGridCellArray)
    boxGridMapper = vtk.vtkPolyDataMapper()
    boxGridMapper.SetInputData(boxGridData)
    actor = vtk.vtkActor()
    actor.SetMapper(boxGridMapper)
    if color is not None:
        actor.GetProperty().SetColor(color)
    return actor


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    ori_surface_file = os.path.join(full_path, 'orignal_'+model+'.stl')
    def_surface_file = os.path.join(full_path, 'deformed_'+model+'.stl')
    def_mask_file = os.path.join(full_path, 'deformed_mask_'+model+'_' + str(inc) + 'mm.mat')
    image_file = os.path.join(full_path,  model+'.mat')
    dat = scio.loadmat(image_file)
    image3D = dat['image']
    #image3D = np.flip(image3D, axis=2)
    #image3D = np.transpose(image3D, (1, 0, 2))  # only for duke
    dat = scio.loadmat(def_mask_file)
    def_image3D = dat['image']
    origin = dat['origin']
    #deimage3D = np.flip(deimage3D, axis=2)

    '''
    deforming ...
    '''

    translates = scio.loadmat(os.path.join(full_path,'coo_mtx_body_solve_'+str(inc)+'mm.mat'))
    translate_x = np.squeeze(translates['translate_x'])
    translate_y = np.squeeze(translates['translate_y'])
    translate_z = np.squeeze(translates['translate_z'])

    data = scio.loadmat(os.path.join(full_path,'Surf_idx_body_'+str(inc)+'mm.mat'))
    tissue_coord3D = data['nnz_coors_3D']
    nnz_points = data['nnz_points']
    nnz = tissue_coord3D.shape[0]

    image3D_sampled = image3D[0:-1:inc, 0:-1:inc, 0:-1:inc]
    shape = image3D.shape
    # def_image3D[0:shape[0], int(920/inc):shape[1], 0:shape[2]] = 0
    # def_image3D[0:shape[0], int(920/inc):shape[1], 0:shape[2]] = image3D_sampled[0:shape[0], np.int(920/inc):shape[1], 0:shape[2]]

    for i in tqdm(range(nnz)):
        p = tissue_coord3D[i, :]
        p1 = nnz_points[i, :]
        x = int(round(p1[0]+translate_x[i]))
        y = int(round(p1[1]+translate_y[i]))
        z = int(round(p1[2]+translate_z[i]))

        # x = round(p[0])
        # y = round(p[1])
        # z = round(p[2])
        if x >= shape[0] or y >= shape[1] or z >= shape[2]:
            print('Error Position (%d, %d, %d) exceed (%d, %d, %d)' % (x, y, z, shape[0], shape[1], shape[2]))
        else:
            def_image3D[int(p[0]), int(p[1]), int(p[2])] = image3D[x, y, z]

    scio.savemat(os.path.join(full_path, 'deformed_'+model+'_image_'+str(inc)+'mm.mat'), {'ima': def_image3D})
    volumeRender_1(def_image3D)


