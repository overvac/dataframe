#include "dataframe/dataframe.h"

int main()
{
	/*
	*	SAMPLE DATA
	*/

	std::vector<bool> data_bool = { true, false, true, false, true };
	std::vector<int> data_int = { 1,2,3,4,5 };
	std::vector<double> data_double = { 1.0, 2.0, 3.0, 4.0, 5.0 };
	std::vector<std::string> data_string = { "one", "two", "three", "four", "five" };
	std::vector<std::string> column_names = { "ex_data_string", "im_data_string" };

	/*
	*	HOW TO USE
	*/

	c_dataframe df;															// creates an empty dataframe
	c_dataframe df_with_headers("sample.csv");								// loads existing "sample.csv" into dataframe
	c_dataframe df_without_headers("sample.csv", false);					// loads existing "sample.csv" into dataframe without headers

	df.add_column<bool>("ex_data_bool", data_bool);							// explicitly creates column with boolean data named "ex_data_bool"
	df.add_column<int>("ex_data_int", data_int);							// explicitly creates column with integer data named "ex_data_int"
	df.add_column<double>("ex_data_double", data_double);					// explicitly creates column with double data named "ex_data_double"
	df.add_column<std::string>("ex_data_string", data_string);				// explicitly creates column with string data named "ex_data_string"

	df.add_column("im_data_bool", data_bool);								// implicitly creates column with boolean data named "im_data_bool"
	df.add_column("im_data_int", data_int);									// implicitly creates column with integer data named "im_data_int"
	df.add_column("im_data_double", data_double);							// implicitly creates column with double data named "im_data_double"
	df.add_column("im_data_string", data_string);							// implicitly creates column with string data named "im_data_string"

	df.remove_column("ex_data_bool");										// removes the column with name "ex_data_bool" from the dataframe
	df.remove_column("im_data_int");										// removes the column with name "im_data_int" from the dataframe

	df.rename_column("ex_data_double", "new_ex_data_double");				// renamed the column from "ex_data_double" to "new_ex_data_double"

	df.dropf();																// drops first row in the dataframe
	df.dropb();																// drops last row in the dataframe
	df.drop(3);																// drops the third row in the dataframe
	df.dropna();															// drops all rows where NaN (not a number) value is present
	df.dropinf();															// drops all rows where INF (infinite) value is present
	df.dropemp();															// drops all rows where some value is empty

	df.at<double>("im_data_double");										// gets values from "im_data_double" column as vector<double>
			
	df.at<std::string>(column_names);										// gets values from multiple columns "column_names" as vector<vector<std::string>>

	df.from_csv("sample2.csv");												// loads existing "sample.csv" into dataframe
	df.from_csv("sample2.csv", false);										// loads existing "sample.csv" into dataframe without headers
	
	df.to_csv("output.csv");												// saves dataframe to "outputs.csv"
	df.to_csv("output.csv", false);											// saves dataframe to "outputs.csv" without headers

	auto shape = df.shape();												// returns pair {number of rows, number of columns} in the dataframe

	df.sum("ex_data_int");													// returns the sum of all values in the "ex_data_int" column
	df.sum("ex_data_int", 2, 5);											// returns the sum of values in the "ex_data_int" column from index 2 to index 5

	df.prod("ex_data_int");													// returns the multiplication product of all values in the "ex_data_int" column
	df.prod("ex_data_int", 2, 5);											// returns the multiplication product of values in the "ex_data_int" column from index 2 to index 5

	df.mean("ex_data_int");													// returns the mean(average) of all values in the "ex_data_int" column
	df.mean("ex_data_int", 2, 5);											// returns the mean(average) of values in the "ex_data_int" column from index 2 to index 5

	df.var("ex_data_int");													// returns the variance of all values in the "ex_data_int" column
	df.var("ex_data_int", 2, 5);											// returns the variance of values in the "ex_data_int" column from index 2 to index 5

	df.std("ex_data_int");													// returns the standard deviation of all values in the "ex_data_int" column
	df.std("ex_data_int", 2, 5);											// returns the standard deviation of values in the "ex_data_int" column from index 2 to index 5

	df.min("ex_data_int");													// returns the minimum value in the "ex_data_int" column
	df.min("ex_data_int", 2, 5);											// returns the minimum value in the "ex_data_int" column from index 2 to index 5

	df.max("ex_data_int");													// returns the maximum value in the "ex_data_int" column
	df.max("ex_data_int", 2, 5);											// returns the maximum value in the "ex_data_int" column from index 2 to index 5

	df.cov("ex_data_int", "im_data_double");								// calculates the covariance between columns "ex_data_int" and "im_data_double"
	df.cov("ex_data_int", "im_data_double", 2, 5);							// calculates the covariance between columns "ex_data_int" and "im_data_double" from index 2 to index 5

	df.corr("ex_data_int", "im_data_double");								// calculates the correlation between columns "ex_data_int" and "im_data_double"
	df.corr("ex_data_int", "im_data_double", 2, 5);							// calculates the correlation between columns "ex_data_int" and "im_data_double" from index 2 to index 5

	df.skew("ex_data_int");													// returns the skewness of all values in the "ex_data_int" column
	df.skew("ex_data_int", 2, 5);											// returns the skewness of values in the "ex_data_int" column from index 2 to index 5

	df.kurt("ex_data_int");													// returns the kurtosis of all values in the "ex_data_int" column
	df.kurt("ex_data_int", 2, 5);											// returns the kurtosis of values in the "ex_data_int" column from index 2 to index 5

	df.diff("ex_data_int");													// returns differences between each value in "ex_data_int" column as vector<double>
	df.diff("ex_data_int", 2, 5);											// returns differences between each value in "ex_data_int" column from index 2 to index 5 as vector<double>

	df.pct_change("ex_data_int");											// returns percent change between each value in "ex_data_int" column as vector<double>
	df.pct_change("ex_data_int", 2, 5);										// returns percent change between each value in "ex_data_int" column from index 2 to index 5 as vector<double>

	df.log_change("ex_data_int");											// returns logarithmic change between each value in "ex_data_int" column as vector<double>
	df.log_change("ex_data_int", 2, 5);										// returns logarithmic change between each value in "ex_data_int" column from index 2 to index 5 as vector<double>

	df.cumsum("ex_data_int");												// returns cumulative sum of values in "ex_data_int" column as vector<double>
	df.cumsum("ex_data_int", 2, 5);											// returns cumulative sum of values in "ex_data_int" column from index 2 to index 5 as vector<double>

	df.cumprod("ex_data_int");												// returns cumulative multiplication product of values in "ex_data_int" column as vector<double>
	df.cumprod("ex_data_int", 2, 5);										// returns cumulative multiplication product of values in "ex_data_int" column from index 2 to index 5 as vector<double>

	df.reset();																// clears and resets the dataframe

	df.head();																// takes the top 5 rows of the dataframe and creates a new dataframe with those values
	df.head(100);															// takes the top 100 rows of the dataframe and creates a new dataframe with those values

	df.tail();																// takes the bottom 5 rows of the dataframe and creates a new dataframe with those values
	df.tail(100);															// takes the bottom 100 rows of the dataframe and creates a new dataframe with those values

	df.print();																// prints/displays the top 5 rows of the dataframe along with column names
	df.print(500);															// prints/displays the top 500 rows of the dataframe along with column names
}