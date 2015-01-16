
package StreamingGUI;

import java.sql.DriverManager;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import XMLTree.DBInterface;
import XMLTree.SqlResultsSetInterface;


public class DBMSImplementation extends DBInterface

{

//  Connection                _conn [];


  public DBMSImplementation ()
  {
    // super ();
    /*
    System. out. println ("CONNECT...........");
    String user = "guier24";
    String pwd =  "guier24";
    String cmy = "world";
    String host = "legnano.sodalia.it";
    String sid = "stmm";
    String OraclePort =  "1521";
    String connect_string = new String("jdbc:oracle:thin:");
    connect_string +=user;
    connect_string += "/";
    connect_string += pwd;
    connect_string += "@(description=(address=(community=";
    connect_string += cmy;
    connect_string += ")(protocol=tcp)(host=";
    connect_string += host;
    connect_string += ")(port=";
    connect_string += OraclePort;
    connect_string += "))(connect_data=(sid=";
    connect_string += sid;
    connect_string += ")(GLOBAL_NAME =";
    connect_string += sid;
    connect_string += ".";
    connect_string += cmy;
    connect_string += ")))";

    try
    {
      _conn         = new Connection [1];
      System. out. println ("1...........");

      DriverManager.registerDriver(new oracle.jdbc.driver.OracleDriver());
      System. out. println ("2...........");
      System. out. println (connect_string);

      _conn [0]     = DriverManager.getConnection(connect_string);
    }
    catch(SQLException e)
    {
      System. out. println ("SQLException..........." + e);
    }
    */
  }

  public SqlResultsSetInterface executeSql (String pSqlStatement,
    int iRowsNumberEachFetch)
  {
    SqlResultsSetImplementation             srsSqlResultsSetImplementation    =
      new SqlResultsSetImplementation ();

    /*
    try
    {
      Statement         stStatement;
      ResultSet         rsResultsSet    = null;

      System. out. println (pSqlStatement);
      stStatement             = (_conn[0]).createStatement();
      rsResultsSet            = stStatement.executeQuery(pSqlStatement);

      srsSqlResultsSetImplementation.init (rsResultsSet);

      rsResultsSet.close ();
      stStatement.close();
    }
    catch(SQLException e)
    {
      System. out. println ("SQLException..........." + e);
    }
    */

    return srsSqlResultsSetImplementation;
  }
}
