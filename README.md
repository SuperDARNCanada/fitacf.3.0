# fitacf.3.0
The repo for the new and improved fitacf routine

##Building the project
Requires a version of RST to be set up with correct environment variables. Aftering running `make` in the project directory, a version of make_fit will be placed in a bin directory within the project folder.

##Details

fitacf.3.0 is a complete rewrite of the ACF fitting routine using in the RST software package. fitacf.3.0 attempts to improve on many aspects of the current version, fitacf.2.7 both in terms of algorithm correctness, and software design.

###Software design

fitacf.3.0 was designed to be easy to read,easy to test, and easy to modify. In contrast to the older versions, variables and function names, and files are more self descriptive so that it is easier to locate things and understand the code. Data is now better [encapsulated](https://en.wikipedia.org/wiki/Encapsulation_(computer_programming)) so that it is clear what is being operated on at all times. And because data follows a better encapsulation scheme, it means that functions can be designed with greatly reduced [coupling](https://en.wikipedia.org/wiki/Coupling_(computer_programming)) compared to the older versions.

The data structure used to contain data during the fitting procedure is a nested [linked list](https://en.wikipedia.org/wiki/Linked_list). A linked list of range nodes are used to hold the data associated with each range for a particular scan. Range nodes hold within themselves quite a bit of information:
+ range number
+ pointer to array of CRI values for each pulse
+ refractive index to optionally use during parameter determination
+ linked list of alpha values for each lag
+ linked list of phase nodes for each lag. Phase nodes described below.
+ linked list of power nodes for each lag. Power nodes descibed below.
+ linked list of elevation nodes for each lag. Uses same structure as phase nodes.
+ pointer to structure containing data for a linear power fit. Fitting structure described further below.
+ pointer to a structure containing data for a quadratic power fit. Fitting structure described further below.
+ pointer to a structure containing data for linear power fit errors. Fitting structure described further below.
+ pointer to a structure containing data for phase fit. Fitting structure described further below.
+ pointer to a structure containing data for the elevation fit. Fitting structure described further below.

Power nodes contain a value for log power, error(sigma), and time. Phase and elevation nodes contain a value for phi, error(sigma), and time. 

The reason linked lists work so well in this application is that although they are more complicated than arrays, it's much easier to add or remove elements without having to `malloc` new sections of contiguious memory and copy values over, or having to use extra arrays to keep track of good data. In this case, when the data structure is run through filtering stages, data can be completely trimmed from the data structure so that one can reliably know that all data left at the fitting and determination stage is good data. Data is almost always worked on sequentially which lists are good at. All fitting structures are defined in *fit_structures.h* 

Using a structure like this means that functions no longer have to be coupled the way they were in older versions of fitacf. Instead of rippling changes from one function to the next, functions return to the top of the stack when they are finished operating on the data structure. This means that we can independently test, modify, or even disable pieces of the program without affecting the operation of further stages. As an example, filtering of transmitter pulse overlapped lags can be completely disabled if a researcher wanted to test something using simulated data. Making changes like this would be extremely difficult in the older versions.

Now that the data structure is explained, we can go into more detail of how this structure is operated on. The program goes through three main stages:

1. Preprocessing
2. Fitting
3. Echo parameter determination

In the preprocessing stage, raw data is read in and the data structure lists and fields are filled using the raw data. Data then goes through a filtering process where low power lags or transmitter pulse overlapped lags are removed, and noise ranges are removed.

In the fitting stage, the power fitting is done first. After a power fit is complete, that data can be used to calculate sigma for the phase nodes and a phase unwrap can occur. Once the phase is unwrapped, a fit can be applied. The process of fitting for elevation is the same as fitting for phase. 

The fitting routine uses the exact algorithms for 1 or 2 parameter straight line least squares fitting as described in *NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING*, and the fitting structure uses the exact same naming convention from this book. If you are looking more in depth to the least squares fitting, or if you would like to use any of the additional information calculated, but not used(coefficient of correlation, chi squared value, etc) then refer to this book.

In the echo parameter determination stage, the fitted values are then used to determine values for things such as velocity, elevation, power, and spectral width with their respective errors. These parameters are then written out to a file.



