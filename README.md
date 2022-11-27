# FastMedianFilter

Implementation of fast median filter based on color histogram.
<br/>

# The idea
The classic algorithm of median filter based on sorting can dramatically slow down processing speed.

So we will use statistics to improve the situation.<br/>
Let's suppose we have an image of size *M×N* pixels and some kernel (a neighbourhood of considered pixel aka. window) of size *K×K*, where *M* and *N* are some natural numbers, *K* is some natural odd number.

Then do the next steps:
1. Take half of neighbourhood size ( (K×K) / 2 );
1. Select a neighbourhood for pixel;
1. Build a histogram for the selected neighbourhood;
1. Sum frequencies for each intensity in histogram until we reach the half of neighbourhood size (calculated in Step 1);
1. The result will be an intensity on which we were reached the half of neighbourhood size;
1. Repeat for each pixel of image.

Note:
> There is no reason to fully recalculate neighbourhood histogram for each pixel within one row.
> Just remove the first column of current neighbourhood and add the new one to the end.
> However, when we are moving from one image row to another, there is no way to avoid entire recalculation.

<br/>

# Example
Let's consider the next neighbourhood:
```
12  3   6
4   4   8
3   4   1
```
then build a histogram from it:

<img src="illustrations/MedianFilterHistogramIllustration.jpg " width="100%" height="100%">

If we have done all the steps mentioned above we will have the median intensity of 4 as a result.

We can check it by sorting neighbourhood values ascending and take the middle one:
```
1 3 3 4 [4] 4 6 8 12
```
Obviously we will have the same result.
 <br/>
 
 # Screenshots
 <img src="illustrations/Illustration1.png" width="100%" height="100%">
 <img src="illustrations/Illustration2.png" width="100%" height="100%">
