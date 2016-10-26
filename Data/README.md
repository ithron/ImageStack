# ImageStack
## Test data files
This folder contains several example / test images and masks.

Axis are in the order X, Y, Z, where

  * the X-Y plane is the transverse plane,
  * the Y-Z plane is the sagittal plane,
  * the X-Z plane is the coronal plane.

The axis directions are

  * the X-axis goes from left to right,
  * the Y-axsis goes from anterior to posterior,
  * the Z-axis goes from caudal to cranial

### zero_Slices.bst
An image with all voxels set to zero

  * Size 20 x 40 x 10
  * Voxel Size [mm] 1 x 2 x 4
### zero_Mask.bst
A mask with all voxels set to zero

  * Size 20 x 40 x 10
  * Voxel Size [mm] 1 x 2 x 4
### ones_Slices.bst
An image with all voxels set to one

  * Size 20 x 40 x 10
  * Voxel Size [mm] 1 x 2 x 4
### ones_Mask.bst
A mask with all voxels set to one
  * Size 20 x 40 x 10
  * Voxel Size [mm] 1 x 2 x 4
### ascending_Slices.bst
An image with ascending volxel values in the range [-2000.0, 1999.5].
The values increase by 0.5 with each voxel in Y-direction, by
20.0 in the X-direction and by 400.0 in the Z-direction.

  * Size 20 x 40 x 10
  * Voxel Size [mm] 0.25 x 0.5 x 1.0
### ascending_Mask.bst
A mask with ascending volxel values in the range [-128, 127].  The values start
by -129 and increas by `d := 255/7999` with each voxel in Y-direction, by `40 *
d` in the X-direction and by `800 * d` in the Z-direction. All values are
rounded to the nearest integer.

  * Size 20 x 40 x 10
  * Voxel Size [mm] 0.25 x 0.5 x 1.0

