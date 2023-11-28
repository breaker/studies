/*
	A script for exporting spline data.
	Usage:
		Copy the script to 3dsmax "plugins" folder and type
		to the Listener:
		g_taoExportShapes()
			to output to the Listener window or
		g_taoExportShapes "c:\\foo.txt"
			to export to a file.
		
	Requires 3dsmax9 or newer (older max revisions requite the Avg extensions by Larry Minton).
	Visit www.tech-artists.org for more.
*/

global g_taoExportShapes

fn g_taoExportShapes strOutputFilename:undefined =
(
	fn getScale =
	(
		fValue = undefined
		case units.systemType of
		(
			#inches:
				fValue = 2.54
			
			#centimeters:
				fValue = 100.0
			
			#meters:
				fValue = 1.0
			
			default:
				fValue = 1.0
		)
		return fValue
	)
	
	-- use the same formatting for all float values
	fn _formattedPrint f =
	(
		return (formattedPrint f format:" .8f")
	)
	
	-- variables
	local iVersion = 1
	local fScale = getScale()
	local strAxisOrder = "right up front"	-- not really supporting any axis order, but at least writing out the one used
	
	local shapeList = #()
	local strOutput = stringStream ""
	
	--  collect all shapes
	for o in shapes do
	(
		if ( classOf o.baseObject == Line ) do
		(
			append shapeList o
		)
	)
	
	-- output data to a stringStream
	format "ShapeDataVersion: %\n" iVersion to:strOutput
	format "AxisOrder: %\n" strAxisOrder to:strOutput
	format "ShapeCount: %\n" shapeList.count to:strOutput

	for iShape = 1 to shapeList.count do
	(
		o = shapeList[iShape]
		
		format "  Shape: % \"%\"\n" iShape o.name to:strOutput
		
		iSplineCount = numSplines o
		format "    SplineCount: %\n" iSplineCount to:strOutput
		
		for iSpline = 1 to iSplineCount do
		(
			format "      Spline: %\n" iSpline to:strOutput
			
			iKnotCount = numKnots o iSpline
			format "        KnotCount: %\n" iKnotCount to:strOutput
			
			for iKnot = 1 to iKnotCount do
			(
				v = getKnotPoint o iSpline iKnot
				v /= fScale
				
				-- y and z get flipped on the output
				format "          Knot: % % %\n" \
					( _formattedPrint v.x ) \
					( _formattedPrint v.z ) \
					( _formattedPrint v.y ) \
					to:strOutput
			)
		)
	)
	
	-- if no filename was provided, output to Listener window
	if ( ( strOutputFilename == undefined) or (strOutputFilename == unsupplied ) ) then
		format "%\n" strOutput
	else
	(
		-- writing to a temp file, just being paranoid in case something fails
		strTempOutputFile = getDir #autoBack + "\\" + ( filenameFromPath strOutputFilename )
		outputHandle = createFile strTempOutputFile
		format (strOutput as string) to:outputHandle
		close outputHandle

		bFileExists = doesFileExist strOutputFilename
		if bFileExists do
		(
			bDeleteSuccess = deleteFile strOutputFilename
			if ( not bDeleteSuccess ) do
			(
				strErrorMessage = "Failed to delete output file:\n" + strOutputFilename + "\n\nSomething bad may happen.\n"
				MessageBox strErrorMessage

			)
		)
		
		copyFile strTempOutputFile strOutputFilename
	)
)

-- Some usage examples:
--g_taoExportShapes()
g_taoExportShapes strOutputFilename:"c:\\foo.txt"
