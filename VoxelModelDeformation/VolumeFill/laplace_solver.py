import numpy as np
import os
import scipy.sparse.linalg
from scipy.sparse import csr_matrix
from scipy.sparse.linalg import bicgstab
from scipy.sparse import coo_matrix
from threading import Thread, Lock
import scipy.io as scio
from scipy.io import mmread, mmwrite, mminfo
import time
import inspect
import warnings
from tqdm import tqdm
from matplotlib import pyplot as plt

plt.rcParams['font.sans-serif']=['SimHei'] #显示中文

inc = 2

def report(xk):
    frame = inspect.currentframe().f_back
    print(frame.f_locals['resid'])

model = 'Leg'
full_path = 'F:\\UniformMF\\Human_models\\'+model +'_refinement'
inc = 2

filename = os.path.join(full_path, 'coo_mtx_body_'+str(inc)+'mm.mat')
data = scio.loadmat(filename)
phi_x = np.squeeze(data['translate_x']).astype('float')
phi_y = np.squeeze(data['translate_y']).astype('float')
phi_z = np.squeeze(data['translate_z']).astype('float')
rows = np.squeeze(data['coo_row']).astype('int')
cols = np.squeeze(data['coo_col']).astype('int')
vals = np.squeeze(data['coo_val']).astype('float')
mtx_size = np.squeeze(data['mtx_size'])

coo_mtx = coo_matrix((vals, (rows, cols)), shape=(mtx_size[0], mtx_size[1]))
A = coo_mtx.tocsr()
x0 = phi_x
b = np.squeeze(np.zeros((mtx_size[0], 1)))


translate_x, info = bicgstab(A=A, b=phi_x, x0=phi_x, callback=report, tol=1e-12)
translate_y, info = bicgstab(A=A, b=phi_y, x0=phi_y, callback=report, tol=1e-12)
translate_z, info = bicgstab(A=A, b=phi_z, x0=phi_z, callback=report, tol=1e-12)


scio.savemat(os.path.join(full_path, 'coo_mtx_body_solve_'+str(inc)+'mm.mat'), {'translate_x': translate_x,
                                                 'translate_y': translate_y,
                                                 'translate_z': translate_z})

