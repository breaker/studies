/*
	A script for exporting spline data as binary.
	Usage:
		Copy the script to 3dsmax "plugins" folder and type
		to the Listener:
		g_taoExportShapesAsBinary()
			to output to the Listener window or
		g_taoExportShapesAsBinary "c:\\foo.bin"
			to export to a file.
		
	Requires 3dsmax9 or newer (older max revisions requite the Avg extensions by Larry Minton).
	Visit www.tech-artists.org for more.
*/

global g_taoExportShapesAsBinary

fn g_taoExportShapesAsBinary strOutputFilename:undefined =
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
	
	-- variables
	local iVersion = 1
	local fScale = getScale()

	local shapeList = #()

	--  collect all shapes
	for o in shapes do
	(
		if ( classOf o.baseObject == Line ) do
		(
			append shapeList o
		)
	)
	
	if( ( strOutputFilename == undefined ) or ( strOutputFilename == unsupplied ) ) do
		strOutputFilename = "C:\\output.bin"
		
	outputStream = fOpen strOutputFilename "wb"
	
	writeLong outputStream iVersion

	-- go thru all shapes and count the number of splines in them
	iSplineCount = 0
	for iShape = 1 to shapeList.count do
	(
		o = shapeList[iShape]
		iSplineCount += numSplines o
	)

	writeLong outputStream iSplineCount

	for iShape = 1 to shapeList.count do
	(
		o = shapeList[iShape]
		for iSpline = 1 to numSplines o do
		(
			iKnotCount = numKnots o iSpline
			writeLong outputStream iKnotCount
			
			for iKnot = 1 to iKnotCount do
			(
				v = getKnotPoint o iSpline iKnot
				v /= fScale
				
				-- y and z get flipped on the output
				writeFloat outputStream v.x
				writeFloat outputStream v.z
				writeFloat outputStream v.y
			)
		)
	)
	fFlush outputStream
	fClose outputStream
	format "Wrote '%'...\n" strOutputFilename
)

-- Some usage examples:
--g_taoExportShapes()
g_taoExportShapesAsBinary strOutputFilename:"c:\\foo.bin"
