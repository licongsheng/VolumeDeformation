#include "stdlib.h"
#include "stdio.h"
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h> 
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkPoints.h>
#include <vtkMath.h>
#include <math.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkPointData.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>


void readSurffile(const char *filename,vtkPolyData *data);
void VoxelerPolyDataSurface(vtkPolyData *pd,double resolution[3],vtkImageData *volume,const char *filename_dim,const char *filename_ima);
void WriteImageFile(const char *filename_dim,const char *filename_ima,vtkImageData *data);

double scale = 1;



int main()
{
	double spacing[3] = {1,1,1};
	double bounds[6] = {0};
	int dimensions[3] = {-1};
	vtkImageData *volume = NULL;
	FILE *fpin = NULL;
	const char *obj_filename =
		"D:\\2017 URSI\\LvBin Head Model\\Head_skin_remeshed.stl";
	const char *filename_dim =
		"D:\\2017 URSI\\LvBin Head Model\\Head_skin.dim";
	const char *filename_ima =
		"D:\\2017 URSI\\LvBin Head Model\\Head_skin.ima";
	/***********************************************************************************/
	vtkPolyData *object = vtkPolyData::New();
	printf("Root will load <%s>\n",obj_filename);
	readSurffile(obj_filename,object);
	printf("Root load <%s> finished.\n",obj_filename);
	object->GetBounds(bounds);
	printf("Object boundary is X: %5.3f ~ %5.3f.\n     Y: %5.3f ~ %5.3f.\n     Z: %5.3f ~ %5.3f.\n",
		bounds[0],bounds[1],bounds[2],bounds[3],bounds[4],bounds[5]);

	dimensions[0] = (int)ceil(bounds[1] - bounds[0])/spacing[0]+1;
	dimensions[1] = (int)ceil(bounds[3] - bounds[2])/spacing[1]+1;
	dimensions[2] = (int)ceil(bounds[5] - bounds[4])/spacing[2]+1;
	printf("Volume data dimension is (%d x %d x %d).\n",dimensions[0],dimensions[1],dimensions[2]);

	volume = vtkImageData::New();
	VoxelerPolyDataSurface(object,spacing,volume,filename_dim,filename_ima);
	//WriteImageFile(filename_dim,filename_ima, volume);

	volume->Delete();
	object->Delete();
	printf("I am finished my work. Converter!!!!");

	getchar();
	return 1;
}

void readSurffile(const char *filename,vtkPolyData *data)
{
	double bounds[6] = {0};
	double dimensions[3] = {-1};
	double spacing[3] = {1,1,1};
	vtkSmartPointer<vtkSTLReader> sr = vtkSmartPointer<vtkSTLReader>::New();
	sr->GlobalWarningDisplayOff();
	sr->SetFileName(filename);
	sr->Update();

	vtkSmartPointer<vtkTransformPolyDataFilter> filter= vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Scale(scale,scale,scale);
	filter->SetInputData(sr->GetOutput());
	filter->SetTransform(transform);
	filter->Update();
	filter->GetOutput()->GetBounds(bounds);
	dimensions[0] = ceil(bounds[1] - bounds[0])/spacing[0]+1;
	dimensions[1] = ceil(bounds[3] - bounds[2])/spacing[1]+1;
	dimensions[2] = ceil(bounds[5] - bounds[4])/spacing[2]+1;
	transform = NULL;
	transform = vtkSmartPointer<vtkTransform>::New();
	transform->Translate(-dimensions[0]/2.0-bounds[0],-dimensions[1]/2.0-bounds[2],-dimensions[2]/2.0-bounds[4]);
	//transform->RotateY(90);
	transform->RotateZ(90);
	data->DeepCopy(filter->GetOutput());
	//data->Update();

	filter = NULL;
	filter= vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	filter->SetInputData(data);
	filter->SetTransform(transform);
	filter->Update();

	data->DeepCopy(filter->GetOutput());
	//data->Update();
	sr = NULL;filter = NULL;transform = NULL;
}

void VoxelerPolyDataSurface(vtkPolyData *pd,double resolution[3],vtkImageData *volume,const char *filename_dim,const char *filename_ima)
{
	double bounds[6] = {0};
	vtkSmartPointer<vtkImageData> whiteImage = 	vtkSmartPointer<vtkImageData>::New(); 
	pd->GetBounds(bounds);
	double spacing[3]; // desired volume spacing
	spacing[0] = resolution[0];
	spacing[1] = resolution[1];
	spacing[2] = resolution[2];
	whiteImage->SetSpacing(spacing);

	// compute dimensions
	int dim[3];
	for (int i = 0; i < 3; i++)
		dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));
	whiteImage->SetDimensions(dim);
	whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

	double origin[3];
	origin[0] = bounds[0] + spacing[0] / 2;
	origin[1] = bounds[2] + spacing[1] / 2;
	origin[2] = bounds[4] + spacing[2] / 2;
	whiteImage->SetOrigin(origin);
	//whiteImage->SetScalarTypeToUnsignedChar();
	whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	// fill the image with foreground voxels:
	unsigned char inval = 200;
	unsigned char outval = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; i++)
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i,inval);

	// polygonal data --> image stencil:
	vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
	pol2stenc->SetInputData(pd);
	pol2stenc->SetOutputOrigin(origin);
	pol2stenc->SetOutputSpacing(spacing);
	pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
	pol2stenc->Update();

	//cut the corresponding white image and set the background:
	vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
	imgstenc->SetInputData(whiteImage);
	imgstenc->SetStencilData(pol2stenc->GetOutput());
	imgstenc->ReverseStencilOff();
	imgstenc->SetBackgroundValue(outval);
	imgstenc->Update();
	pol2stenc = NULL;whiteImage = NULL;
	vtkImageData *data = imgstenc->GetOutput();
	//volume->DeepCopy(imgstenc->GetOutput());
	//const char *filename_dim ="D:\\Papers And Works PHD\\Chinese Infant Modeling\\Chinese female Defromation 2013-12-30\\Baby in mom\\Bone_14_1_7.dim";
	//const char *filename_ima ="D:\\Papers And Works PHD\\Chinese Infant Modeling\\Chinese female Defromation 2013-12-30\\Baby in mom\\Bone_14_1_7.ima";
	WriteImageFile(filename_dim,filename_ima, data);

}

void WriteImageFile(const char *filename_dim,const char *filename_ima,vtkImageData * data)
{
		int i,j,k,n;
		FILE *fpin;
		int dims[3] = {0}; 
		double spacing[3] = {0.1,0.1,0.1};
		unsigned char *Data = NULL;
		unsigned char *p;
		int x1,x2,y1,y2,z1,z2;
		printf("SAVE IMA FILE -- %s \n           -- %s\n",filename_ima,filename_dim);
		fpin = fopen(filename_dim,"w");
		data->GetDimensions(dims);
		//data->GetSpacing(spacing);
		fprintf(fpin,"%d %d %d\n-ty pe U8\n",dims[0],dims[1],dims[2]);
		fprintf(fpin,"-dx %7.4f\n",spacing[0]);
		fprintf(fpin,"-dy %7.4f\n",spacing[1]);
		fprintf(fpin,"-dz %7.4f\n",spacing[2]);
		fclose(fpin);
		printf("*** Save dim file finished ...  \n*** Write ima file ...");
		/*------------------------------------------------------------------*/
		data->GetExtent(x1,x2,y1,y2,z1,z2);
		printf("Data Extent is %d %d %d %d %d %d\n",x1,x2,y1,y2,z1,z2);
		fpin = fopen(filename_ima,"wb");
		Data= new unsigned char[dims[0]*dims[1]*dims[2]];
		for(k=z1;k<=z2;k++)
		{
			for(j=y1;j<=y2;j++)
			{
				for(i=x1;i<=x2;i++)
				{
					p = (unsigned char*)(data->GetScalarPointer(i,j,k));
					Data[(k-z1)*(dims[0]*dims[1])+(j-y1)*dims[0]+i-x1]=(unsigned char)(*p);
				} 
			}
			printf("%s ====== %d / %d\n",(char*)filename_ima,k-z1+1,dims[2]);
		}
		n=fwrite(Data,sizeof(unsigned char),dims[0]*dims[1]*dims[2],fpin);
		if(n!=dims[0]*dims[1]*dims[2])
		{
			printf("--Error: Write faild in position (%d,%d,%d)\n",i,j,k);
		}
		fclose(fpin);

		delete []Data;
}