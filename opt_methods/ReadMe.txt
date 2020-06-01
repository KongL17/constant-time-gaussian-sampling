
example 
  folder gives a simple example of the SIAM optimization method.
    1) in-org  file is the original datas for boolean minimization.
    2) comp-rev.py  file is the source code of SIAM optimization.
                    Note that the optimal placement of terminal nodes is automatically written back to the original input file.
    3) in  file is the optimized result after the SIAM method by executing comp-rev.py. 
    4) result.out  file is the optimal placement of the terminal nodes for the original input in-org file.

subtree-org
   file is a simple example of the obtained subtree in the DTC method.

subtree-split 
   file is first optimized by the SPRM method (execute the replacement of the cutting point and the failure point).
   Then we split the subtree into three sections manually. 
   Last the iterations of SIAM method are executed in a bottom-up fashion.

subtree 
    file is the optimized subtree after the SPRM and SIAM method.

result.out 
    file is the optimal placement of the terminal nodes for each iteration in SIAM. 
