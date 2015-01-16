package StreamingGUI;

import java.util.*;
import java.net.*;
import javax.swing.*;
import java.io.*;
import java.awt.Frame;
import java.util.StringTokenizer;
import XMLTree.*;

public class ConfigurationTransaction

{

/*
  public ConfigurationTransaction ()
  {
  }
*/


  static public int beginGUIConfigurationTransaction (
	String sConfigurationFileName, String sUser)

  {

   String                    sConfigurationLockFile;
   BufferedWriter            bwBufferedWriter;
   File                      fFile;

   sConfigurationLockFile   = new String (sConfigurationFileName + ".lck");

   System.out.println("beginGUIConfigurationTransaction: " + sConfigurationFileName);

   try
   {
      fFile = new File (URLDecoder.decode(sConfigurationLockFile, "UTF-8"));
   }
   catch (IOException e)
   {
	   JOptionPane.showMessageDialog(null,
		 "URLDecoder.decode failed",
		 "ConfigurationTransaction",
		 JOptionPane.ERROR_MESSAGE);

	   return 1;
   }

   if (fFile.exists())
   {
	 BufferedReader brBufferedReader;
	 char cLastConfigurationUser [];

	 cLastConfigurationUser         = new char [((int) fFile.length())];

	 try
	 {
	   brBufferedReader = new BufferedReader(new FileReader(
		 URLDecoder.decode(sConfigurationLockFile, "UTF-8")));
	   brBufferedReader.read(cLastConfigurationUser, 0,
		 (int) (fFile.length()));
	   brBufferedReader.close();

	   System.out.println("Last configuration user: " +
	     String.copyValueOf(cLastConfigurationUser));
	 }
	 catch (IOException e)
	 {
	   JOptionPane.showMessageDialog(null,
		 "Operation on BufferedWriter failed (1)",
		 "ConfigurationTransaction",
		 JOptionPane.ERROR_MESSAGE);

	   return 1;
	 }

	 if (String.copyValueOf(cLastConfigurationUser).compareTo(sUser) == 0)
	 {
	   System.out.println("File. delete: " + sConfigurationFileName);

	   if (fFile.delete() == false)
	   {
		 JOptionPane.showMessageDialog(null,
		   "fFile.delete failed", "ConfigurationTransaction",
		   JOptionPane.ERROR_MESSAGE);

		  return 2;
		}
	  }
	  else
	 {
	   JOptionPane.showMessageDialog(null,
		 "The configuration is locked by " +
		 String.copyValueOf(cLastConfigurationUser) +
		 ".\nYou cannot change the configuration.",
		 "ConfigurationTransaction",
		 JOptionPane.PLAIN_MESSAGE);

		return 3;
	  }
   }

   try {
	 bwBufferedWriter = new BufferedWriter(new FileWriter(
	   URLDecoder.decode(sConfigurationLockFile, "UTF-8")));
	 bwBufferedWriter.write(sUser, 0, sUser.length());
	 bwBufferedWriter.close();

	 System.out.println("Configuration user written: " + sUser);
   }
   catch (IOException e) {
	 JOptionPane.showMessageDialog(null,
	   "Operation on BufferedWriter failed (" + e + ")",
	   "ConfigurationTransaction",
	   JOptionPane.ERROR_MESSAGE);

	 return 4;
   }

  return 0;
  }

  static public int commitGUIConfigurationTransaction (
	XMLTreeForConfiguration xtfcXMLTreeForConfiguration,
	Vector vFunctionalities)

  {
	TreeVisit                                tvConfigurationTreeVisit;
	TreeVisitToGetConfigurationFile
	  tvgcTreeVisitToGetConfigurationFile;
	String                                   sConfigurationFile;
	URL                       uXML;
	String                    sConfigurationLockFile;
	BufferedWriter            bwBufferedWriter;
    File                      fFile;


	tvgcTreeVisitToGetConfigurationFile =
	  new TreeVisitToGetConfigurationFile(vFunctionalities);
	tvConfigurationTreeVisit = new TreeVisit(tvgcTreeVisitToGetConfigurationFile);

	if (tvConfigurationTreeVisit.inOrderVisit(
	  xtfcXMLTreeForConfiguration.getXMLTopTreeComponent ()) != 0)
    {
	  JOptionPane.showMessageDialog(null,
		  "inOrderVisit failed", "ConfigurationTransaction",
		  JOptionPane.ERROR_MESSAGE);

	  return 1;
	}

	sConfigurationFile  = tvgcTreeVisitToGetConfigurationFile.getConfigurationFile();

    System.out.println("commitGUIConfigurationTransaction: " + sConfigurationFile);

	uXML     = xtfcXMLTreeForConfiguration.getXML ();

	try {
	   bwBufferedWriter = new BufferedWriter(new FileWriter(
		 URLDecoder.decode(uXML.getFile(), "UTF-8")));
	   bwBufferedWriter.write(sConfigurationFile, 0,
		 sConfigurationFile.length());
	   bwBufferedWriter.close();

	   System.out.println("commitGUIConfigurationTransaction: file written (" +
	     URLDecoder.decode(uXML.getFile(), "UTF-8") + ")");
	}
	catch (IOException e) {
	   JOptionPane.showMessageDialog(null,
		 "Operation on BufferedWriter failed (3)",
		 "ConfigurationTransaction",
		 JOptionPane.ERROR_MESSAGE);

	   return 2;
	}

	sConfigurationLockFile   = new String (uXML.getFile() + ".lck");

   try
   {
      fFile = new File (URLDecoder.decode(sConfigurationLockFile, "UTF-8"));

	  System.out.println("File. delete: " +
	    URLDecoder.decode(sConfigurationLockFile, "UTF-8"));
   }
   catch (IOException e)
   {
	   JOptionPane.showMessageDialog(null,
		 "URLDecoder.decode failed",
		 "ConfigurationTransaction",
		 JOptionPane.ERROR_MESSAGE);

	   return 1;
   }

	if (fFile.delete() == false)
	{
	   JOptionPane.showMessageDialog(null,
		 "fFile.delete on " + sConfigurationLockFile +
		 " failed", "ConfigurationTransaction",
		 JOptionPane.ERROR_MESSAGE);

		return 3;
	}

	return 0;
  }

  static public int rollbackGUIConfigurationTransaction (
	String sConfigurationFileName)

  {
	String                    sConfigurationLockFile;
    File                      fFile;

	sConfigurationLockFile   = new String (sConfigurationFileName + ".lck");

   try
   {
      fFile = new File (URLDecoder.decode(sConfigurationLockFile, "UTF-8"));
   }
   catch (IOException e)
   {
	   JOptionPane.showMessageDialog(null,
		 "URLDecoder.decode failed",
		 "ConfigurationTransaction",
		 JOptionPane.ERROR_MESSAGE);

	   return 1;
   }

	if (fFile.delete() == false)
	{
	   JOptionPane.showMessageDialog(null,
		 "fFile.delete on " + sConfigurationLockFile +
		 " failed", "ConfigurationTransaction",
		 JOptionPane.ERROR_MESSAGE);

		return 3;
	}

	return 0;
  }
}
