tdparse
=======

The files contained here are for the purposes of understanding the differences in tables across servers in Microsoft SQL Server.

##tdparse.exe:	
	This program will quickly read through all the columns in the tablediff output
	and consolidate the ranges on consecutive row differences based on a 
	parameter "min_consecutive_diff" which will specify the minimum criteria
	to be included within the output. Setting min_consecutive_diff=1 will
	yield the same output as the tablediff output, whereas setting it to 100
	will ignore and exclude any ranges of differences in the table less than 100
	consecutive mismatches. The idea is to find a pattern in the data (i.e. this
	column is different for these number of rows, which we know were populated around
	this time. What happened then?)

	Place in the same folder as tablediff.exe, which will look something like
	"C:\Program Files\Microsoft SQL Server\(80,90,100)\COM"

	usage: tdparse.exe infile outfile min_consecutive_diff
	where min_consecutive_diff is optional.
		
##diff_template.txt:
	A template for creating a batch file. Be sure to save it as a .bat when done editing.
	Set the database name, the schema names, and the table names according to the template
	and the examples provided. for the long lists of tables in schemas, 
	notepad++ and it's ctrl+a then ctrl+j will be your friend. (These concatenates separated
	lines into a single space-delimited list. Additionally, set a minimum consecutive 
	difference value. If you do not want a SQL fix file to be generated 
	(fix files can take up a lot of space, delete the line in the nested for loop 
	calling tablediff.exe with a -f parameter.

##src folder:
	contains the source of the program tdparse.exe.

##tablediff documentation: http://msdn.microsoft.com/en-us/library/ms162843.aspx
##tdparse documentation: run tdparse.exe without any arguments.