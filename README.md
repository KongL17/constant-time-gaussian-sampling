**High-Performance Constant-Time Discrete Gaussian Sampling**

This is an implementation of the constant-time Knuth-Yao Gaussian sampling with our proposed DTC method.  

Please note that the DDG tree and the obtained subtrees by the DTC method are all represented in the input form of the tool ESPRESSO.

1. guass-2 
   folder contains the implementations of the constant-time Gaussian samplers using the optimized DTC method (including SPRM and SIAM)
   for the standard deviation of 2. (sampler-64/96/129-opt)

2. opt_methods 
   folder contains source code of the further optimization methods SPRM and SIAM for the DTC method.

3. opt_DTC_subtrees 
   folder contains the obtained subtrees by the DTC method (The cut operation has to be done manually) 
   for the standard deviation of 2 under the precision of 64, 96 and 128. 
   Note that all of these subtrees have been optimized by the further optimizations SPRM and SIAM.

4. node_height 
   folder contains the code to calculate the height of the leftmost second internal node for standard deviation of 3.33. 
   The finiteness of the leftmost second internal nodes can be seen. 
   Based on these heights, the desired subtree depth (depth of 6 and 8 for example) can be achieved by dynamically adjusting cutting steps. 

5. sampler_3_33 
   folder contains the implementations of the constant-time Gaussian samplers using the optimized DTC method 
   for the standard deviation of 3.33 under the precision of 64, 96 and 128.

6. steps_2_to_4 
   folder contains the implementations of the constant-time samplers under three selected cutting steps for the DTC method 
   with the standard deviation 3.33.
