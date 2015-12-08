========================================================================
    CONSOLE APPLICATION : CurvesEditor Project Overview
========================================================================

AppWizard has created this CurvesEditor application for you.

This file contains a summary of what you will find in each of the files that
make up the CurvesEditor application.


CurvesEditor.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

CurvesEditor.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

CurvesEditor.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named CurvesEditor.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////

FEATURES:
- Pressing SPACE makes the next object selected instead of the current one, repeatedly pressing SPACE cycles over all curves.

- When no keys are pressed and the user presses the mouse button, the object (approximately) at the mouse cursor (if there is any) becomes selected. 

- Press 'A' to add control points to the selected object by clicking, then again to stop.

- Press 'D' and click on a point to remove it from the selected curve.

- Press 'P' to create polylines'.

- Press 'L' to create Lagrange curves.

- Press 'B' to create Bezier curves.
