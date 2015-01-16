package StreamingGUI;

import java.awt.*;
import java.io.*;
import java.awt.event.*;
import java.applet.*;
import javax.swing.*;
import java.util.Properties;
import java.util.Hashtable;
import java.util.Date;
import java.net.URL;
import org.omg.CORBA.ORB;
import StreamingIDL.*;
import XMLTree.*;
import java.beans.*;
import javax.swing.event.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2002</p>
 * <p>Company: </p>
 * @author Catrambone Giuliano
 * @version 1.0
 */

public class StreamingGUI extends JApplet {
  boolean isStandalone = false;
  JToolBar jtbStreamingGUI = new JToolBar();
  JTabbedPane jtpStreamingGUI = new JTabbedPane();
  JPanel jpConfigurations = new JPanel();
  BorderLayout borderLayout1 = new BorderLayout();
  JPanel jpContentsMP4Tools = new JPanel();
  BorderLayout borderLayout3 = new BorderLayout();
  JSplitPane jspMonitor = new JSplitPane();
  JPanel jpReports = new JPanel();
  BorderLayout borderLayout4 = new BorderLayout();
  BorderLayout borderLayout5 = new BorderLayout();
  JPanel jpLogs = new JPanel();
  JPanel jpMonitor = new JPanel();
  BorderLayout borderLayout2 = new BorderLayout();
  JScrollPane jspTable = new JScrollPane();
  JTable jtMonitor = new JTable();
  JTextField jtfURLForPlayer = new JTextField();
  JPanel jpMonitorPanel = new JPanel();
  BorderLayout borderLayout6 = new BorderLayout();
  JToolBar jtbMonitor = new JToolBar();
  JLabel jlMonitorTableTitle = new JLabel();
  JComboBox jcbRefreshAutomatic = new JComboBox();
  JLabel jlAutomaticRefresh = new JLabel();
  JButton jbRefresh = new JButton();
  JLabel jlConnectedUsersValue = new JLabel();
  JLabel jlConnectedUsers = new JLabel();


  ORB                           _orb;

  XMLTreeForContents xtfcXMLTreeForContents = new XMLTreeForContents(
    jtfURLForPlayer);
  XMLTreeForMonitor xtfmXMLTreeForMonitor = new XMLTreeForMonitor(
    jlMonitorTableTitle, jtMonitor, jlConnectedUsersValue);
  XMLTreeForConfiguration xtfcXMLTreeForConfiguration =
    new XMLTreeForConfiguration();


  //Get a parameter value
  public String getParameter(String key, String def) {
    return isStandalone ? System.getProperty(key, def) :
      (getParameter(key) != null ? getParameter(key) : def);
  }

  //Construct the applet
  public StreamingGUI() {
/*
	  long index;
	long una   = 0;
	long due   = 0;
	long tre   = 0;
	long quattro   = 0;
	long cinque   = 0;
	long sei   = 0;
	long sette   = 0;
	long otto   = 0;
	long nove   = 0;
	long dieci   = 0;
	long altro   = 0;
	long value;
	String token;
	for (index = 2026439000; index < 50000000000L; index++){
	  // System.out.println (Long.toString(index *1000,Character.MAX_RADIX));
	  value = index * 1;
	  token = Long.toString(value,Character.MAX_RADIX);
	  if (value < 100000)
		System.out.println (index + " " + value + " " + token + " " + token.length());

	  if (token.length() == 1)
		una++;
	  else if (token.length() == 2)
		due++;
	  else if (token.length() == 3)
		tre++;
	  else if (token.length() == 4)
		quattro++;
	  else if (token.length() == 5)
		cinque++;
	  else if (token.length() == 6)
		sei++;
	  else if (token.length() == 7)
	  {
		System.out.println (index + " " + value + " " + token + " " + token.length());
		sette++;
		break;
	  }
	  else if (token.length() == 8)
		otto++;
	  else if (token.length() == 9)
		nove++;
	  else if (token.length() == 10)
		dieci++;
	  else
		altro++;
    }
	System.out.println ("una: " + una);
	System.out.println ("due: " + due);
	System.out.println ("tre: " + tre);
	System.out.println ("quattro: " + quattro);
	System.out.println ("cinque: " + cinque);
	System.out.println ("sei: " + sei);
	System.out.println ("sette: " + sette);
	System.out.println ("otto: " + otto);
	System.out.println ("nove: " + nove);
	System.out.println ("dieci: " + dieci);
	System.out.println ("altro: " + altro);
*/
  }

  //Initialize the applet
  public void init() {
    try {
      jbInit();

	  System.out.println("User home: " + System.getProperty("user.home"));
	  System.out.println("Java home: " + System.getProperty("java.home"));

      _orb = ORB.init(this, null);
      System.out.println ("ORB initialized");

      DBMSImplementation dbrDB = new DBMSImplementation();

      Properties pProperties                           = new Properties ();
    	// pProperties.setProperty("unixId", (getUserHandle() >> 16)+"");
    	// pProperties.setProperty("login", this.getUserName());

      {
		try {
		  LoginDialog ldLoginDialog;

		  ldLoginDialog = new LoginDialog();

		  ldLoginDialog.setVisible(true);

		  if (ldLoginDialog.isDialogSubmitted()) {
			// CHECK the PASSWORD
			if (ldLoginDialog.getUserName().compareTo(new String("guest")) != 0 ||
				ldLoginDialog.getPassword().compareTo(new String("guest")) != 0) {
			  JOptionPane.showMessageDialog(null,
											"login failed", "StreamingGUI",
											JOptionPane.ERROR_MESSAGE);

			  return;
			}

			pProperties.setProperty("login",
									ldLoginDialog.getUserName());
		  }
		  else {
			JOptionPane.showMessageDialog(null,
										  "login failed", "StreamingGUI",
										  JOptionPane.ERROR_MESSAGE);

			return;
		  }
		}
		finally {
		}
	  }

      URL				uXMLForConfigurationUrl;

	  uXMLForConfigurationUrl       =
		  new URL (this.getCodeBase () + this.getParameter("XMLForConfigurationFileName"));
      System.out.println ("XMLForConfigurationFileName: " + uXMLForConfigurationUrl. getFile ());

// inizializzazione della hashtable

      URL				uXMLForContentsUrl       =
        new URL (getCodeBase() + getParameter ("XMLForContentsFileName"));
      System.out.println ("XMLForContentsFileName: " + uXMLForContentsUrl. getFile ());
      xtfcXMLTreeForContents.init (dbrDB,
        uXMLForContentsUrl, pProperties, _orb, xtfcXMLTreeForConfiguration);

      URL				uXMLForMonitorUrl       =
        new URL (getCodeBase() + getParameter ("XMLForMonitorFileName"));
      System.out.println ("XMLForMonitorFileName: " + uXMLForMonitorUrl. getFile ());
      xtfmXMLTreeForMonitor.init (dbrDB,
        uXMLForMonitorUrl, pProperties, _orb);

	  if (ConfigurationTransaction.beginGUIConfigurationTransaction(
		uXMLForConfigurationUrl.getFile(),
		pProperties.getProperty("login")) != 0)
	  {
		JOptionPane.showMessageDialog(null,
		  "beginGUIConfigurationTransaction failed",
		  "StreamingGUI", JOptionPane.ERROR_MESSAGE);

	    return;
	  }

      // the configuration tree will be initialized at the end because it will
      // fill the other trees
      xtfcXMLTreeForConfiguration.init (dbrDB, uXMLForConfigurationUrl, pProperties,
        _orb, xtfcXMLTreeForContents, xtfmXMLTreeForMonitor);
      xtfcXMLTreeForConfiguration.setLastSavedDateForConfigurationItems (new Date ());

	  if (ConfigurationTransaction.commitGUIConfigurationTransaction (
		xtfcXMLTreeForConfiguration,
		xtfcXMLTreeForConfiguration.getFunctionalities()) != 0)
	  {
	    JOptionPane.showMessageDialog(null,
	      "commitGUIConfigurationTransaction failed",
		  "StreamingGUI", JOptionPane.ERROR_MESSAGE);

	    return;
	  }
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  //Component initialization
  private void jbInit() throws Exception {
    // this.setSize(new Dimension(500,400));
    jpConfigurations.setLayout(borderLayout1);
    jpContentsMP4Tools.setLayout(borderLayout3);
    jpReports.setLayout(borderLayout4);
    jpLogs.setLayout(borderLayout5);
    jpMonitor.setLayout(borderLayout2);
    jpMonitorPanel.setLayout(borderLayout6);
    jlMonitorTableTitle.setFont(new java.awt.Font("Dialog", 1, 17));
    jlMonitorTableTitle.setForeground(Color.red);
    jlMonitorTableTitle.setAlignmentX((float) 0.0);
    jlMonitorTableTitle.setHorizontalAlignment(SwingConstants.CENTER);
    jlAutomaticRefresh.setText("          Refresh automatic after (secs): ");
    jbRefresh.setActionCommand("Refresh");
    jbRefresh.setText("Refresh");
    jbRefresh.addActionListener(new StreamingGUI_jbRefresh_actionAdapter(this));
    jcbRefreshAutomatic.addActionListener(new StreamingGUI_jcbRefreshAutomatic_actionAdapter(this));
    jtpStreamingGUI.addChangeListener(new StreamingGUI_jtpStreamingGUI_changeAdapter(this));
    jlConnectedUsersValue.setRequestFocusEnabled(true);
    jlConnectedUsersValue.setText("0");
    jlConnectedUsers.setText("          Connected users: ");
    jtbMonitor.add(jbRefresh, null);
    jtbMonitor.add(jlConnectedUsers, null);
    this.getContentPane().add(jtbStreamingGUI, BorderLayout.NORTH);
    this.getContentPane().add(jtpStreamingGUI,  BorderLayout.CENTER);
    jtpStreamingGUI.add(jpConfigurations,   "Configurations");
    jpConfigurations.add(xtfcXMLTreeForConfiguration,  BorderLayout.CENTER);
    jtpStreamingGUI.add(jpContentsMP4Tools,   "Contents & MP4 tools");
    jpContentsMP4Tools.add(xtfcXMLTreeForContents,  BorderLayout.CENTER);
    jpContentsMP4Tools.add(jtfURLForPlayer, BorderLayout.SOUTH);
    jtpStreamingGUI.add(jspMonitor,   "Monitor");
    jspMonitor.add(xtfmXMLTreeForMonitor, JSplitPane.TOP);
    jspMonitor.add(jpMonitor, JSplitPane.BOTTOM);
    jpMonitor.add(jspTable, BorderLayout.CENTER);
    jpMonitor.add(jpMonitorPanel, BorderLayout.NORTH);
    jpMonitorPanel.add(jtbMonitor,  BorderLayout.CENTER);
    jtbMonitor.add(jlConnectedUsersValue, null);
    jtbMonitor.add(jlAutomaticRefresh, null);
    jtbMonitor.add(jcbRefreshAutomatic, null);
    jpMonitorPanel.add(jlMonitorTableTitle, BorderLayout.NORTH);
    jspTable.getViewport().add(jtMonitor, null);
    jtpStreamingGUI.add(jpReports,   "jpReports");
    jtpStreamingGUI.add(jpLogs,   "jpLogs");
    jtbMonitor.addSeparator();
    jtbMonitor.addSeparator();
    jcbRefreshAutomatic.addItem(new String ("none"));
	jcbRefreshAutomatic.addItem(new Integer (30));
	jcbRefreshAutomatic.addItem(new Integer (45));
	jcbRefreshAutomatic.addItem(new Integer (60));
	jcbRefreshAutomatic.addItem(new Integer (90));
	jcbRefreshAutomatic.addItem(new Integer (120));
  }

  //Start the applet
  public void start() {
  }
  //Stop the applet
  public void stop() {
  }
  //Destroy the applet
  public void destroy() {
  }
  //Get Applet information
  public String getAppletInfo() {
    return "Applet Information";
  }
  //Get parameter info
  public String[][] getParameterInfo() {
    return null;
  }

  //static initializer for setting look & feel
  static {
    try {
      //UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
      //UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
    }
    catch(Exception e) {
    }
  }

  void jbMonitoringRefresh_actionPerformed(ActionEvent e) {
	System.out.println("Refresh pressed");
	xtfmXMLTreeForMonitor.tableRefresh();
  }

  void jcbRefreshAutomatic_actionPerformed(ActionEvent e) {
	if (jcbRefreshAutomatic.getSelectedIndex() == 0)
	  xtfmXMLTreeForMonitor.changeRefreshAutomatic(new Integer (-1));
	else
	  xtfmXMLTreeForMonitor.changeRefreshAutomatic((Integer) jcbRefreshAutomatic.getSelectedItem());
  }

  void jtpStreamingGUI_stateChanged(ChangeEvent e) {
    if (((JTabbedPane) e.getSource()).getSelectedIndex() ==  2)
    {
	  System.out.println("xtfmXMLTreeForMonitor.activeMonitoringThread");
	  xtfmXMLTreeForMonitor.activeMonitoringThread(true);
    }
	else
    {
	  System.out.println("xtfmXMLTreeForMonitor.deactiveMonitoringThread");
	  xtfmXMLTreeForMonitor.activeMonitoringThread(false);
    }
  }
}

class StreamingGUI_jbRefresh_actionAdapter implements java.awt.event.ActionListener {
  StreamingGUI adaptee;

  StreamingGUI_jbRefresh_actionAdapter(StreamingGUI adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jbMonitoringRefresh_actionPerformed(e);
  }
}

class StreamingGUI_jcbRefreshAutomatic_actionAdapter implements java.awt.event.ActionListener {
  StreamingGUI adaptee;

  StreamingGUI_jcbRefreshAutomatic_actionAdapter(StreamingGUI adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jcbRefreshAutomatic_actionPerformed(e);
  }
}

class StreamingGUI_jtpStreamingGUI_changeAdapter implements javax.swing.event.ChangeListener {
  StreamingGUI adaptee;

  StreamingGUI_jtpStreamingGUI_changeAdapter(StreamingGUI adaptee) {
    this.adaptee = adaptee;
  }
  public void stateChanged(ChangeEvent e) {
    adaptee.jtpStreamingGUI_stateChanged(e);
  }
}

