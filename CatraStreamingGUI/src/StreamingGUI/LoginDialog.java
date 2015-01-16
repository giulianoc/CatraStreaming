
package StreamingGUI;

import javax.swing.*;
import java.awt.*;
import javax.swing.border.*;
import java.awt.event.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2002</p>
 * <p>Company: </p>
 * @author Catrambone Giuliano
 * @version 1.0
 */

public class LoginDialog extends JDialog {
  JPanel jPanel3 = new JPanel();
  BorderLayout borderLayout1 = new BorderLayout();
  TitledBorder titledBorder1;
  JLabel jlLogin = new JLabel();
  BorderLayout borderLayout2 = new BorderLayout();
  JPanel jPanel4 = new JPanel();
  JPanel jPanel1 = new JPanel();
  JCheckBox jcbRememberLoginInformation = new JCheckBox();
  JPanel jPanel5 = new JPanel();
  JCheckBox jcbLoginAsGuest = new JCheckBox();
  JPanel jPanel6 = new JPanel();
  JButton jbOK = new JButton();
  JButton jbCancel = new JButton();
  JLabel jlPassword = new JLabel();
  JPanel jPanel2 = new JPanel();
  JPasswordField jpfPassword = new JPasswordField();
  BorderLayout borderLayout3 = new BorderLayout();
  JPanel jPanel7 = new JPanel();
  JLabel jlUserName = new JLabel();
  JTextField jtfUserName = new JTextField();

  String            sUserName;
  String            sPassword;
  boolean           bIsDialogSubmitted;

  public LoginDialog()
  {
    super((Frame) null, "Log in", true);

    try {
      jbInit();
      pack();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }

  private void jbInit() throws Exception {
    titledBorder1 = new TitledBorder("");
    jPanel3.setLayout(borderLayout1);
    jlLogin.setFont(new java.awt.Font("SansSerif", 1, 32));
    jlLogin.setForeground(Color.red);
    jlLogin.setBorder(null);
    jlLogin.setHorizontalAlignment(SwingConstants.CENTER);
    jlLogin.setText("Log In");
    jPanel4.setLayout(borderLayout2);
    jcbRememberLoginInformation.setEnabled(false);
    jcbRememberLoginInformation.setText("Remember login information");
    jcbRememberLoginInformation.addActionListener(new LoginDialog_jcbRememberLoginInformation_actionAdapter(this));
    jcbLoginAsGuest.setText("Log in as guest");
    jcbLoginAsGuest.addActionListener(new LoginDialog_jcbLoginAsGuest_actionAdapter(this));
    jbOK.setText("OK");
    jbOK.addActionListener(new LoginDialog_jbOK_actionAdapter(this));
    jbCancel.setText("Cancel");
    jbCancel.addActionListener(new LoginDialog_jbCancel_actionAdapter(this));
    jPanel1.setLayout(borderLayout3);
    jlPassword.setMaximumSize(new Dimension(50, 15));
    jlPassword.setMinimumSize(new Dimension(50, 15));
    jlPassword.setPreferredSize(new Dimension(100, 15));
    jlPassword.setRequestFocusEnabled(true);
    jlPassword.setText("Password:");
    jpfPassword.setMinimumSize(new Dimension(100, 21));
    jpfPassword.setPreferredSize(new Dimension(100, 21));
    jpfPassword.setToolTipText("");
    jpfPassword.setText("");
    jlUserName.setMaximumSize(new Dimension(49, 15));
    jlUserName.setMinimumSize(new Dimension(49, 15));
    jlUserName.setPreferredSize(new Dimension(100, 15));
    jlUserName.setToolTipText("");
    jlUserName.setText("Username:");
    jtfUserName.setMinimumSize(new Dimension(100, 21));
    jtfUserName.setPreferredSize(new Dimension(100, 21));
    jtfUserName.setText("");
    jPanel6.setDebugGraphicsOptions(0);
    this.getContentPane().add(jPanel3, BorderLayout.CENTER);
    jPanel3.add(jlLogin,  BorderLayout.NORTH);
    jPanel4.add(jPanel1,  BorderLayout.CENTER);
    jPanel2.add(jlPassword, null);
    jPanel2.add(jpfPassword, null);
    jPanel1.add(jPanel2,  BorderLayout.SOUTH);
    jPanel1.add(jPanel7, BorderLayout.NORTH);
    jPanel7.add(jlUserName, null);
    jPanel7.add(jtfUserName, null);
    jPanel4.add(jPanel5,  BorderLayout.SOUTH);
    jPanel5.add(jcbRememberLoginInformation, null);
    jPanel5.add(jcbLoginAsGuest, null);
    jPanel3.add(jPanel6, BorderLayout.SOUTH);
    jPanel6.add(jbCancel, null);
    jPanel6.add(jbOK, null);
    jPanel3.add(jPanel4, BorderLayout.CENTER);
  }

  void jbCancel_actionPerformed(ActionEvent e) {
	sUserName      = null;
	sPassword      = null;
	bIsDialogSubmitted   = false;

	setVisible (false);
	dispose ();
  }

  void jbOK_actionPerformed(ActionEvent e) {
	sUserName      = jtfUserName.getText();
	sPassword      = new String (jpfPassword.getPassword());
	System.out.println(sUserName);
	System.out.println(sPassword);
	bIsDialogSubmitted   = true;

	setVisible (false);
	dispose ();
  }

  boolean isDialogSubmitted () {
	return bIsDialogSubmitted;
  }

  String getUserName () {
	return sUserName;
  }

  String getPassword () {
	return sPassword;
  }

  void jcbRememberLoginInformation_actionPerformed(ActionEvent e) {
  }

  void jcbLoginAsGuest_actionPerformed(ActionEvent e) {
      if (((JCheckBox) e.getSource()).getModel().isSelected())
	  {
	    jtfUserName.setText("guest");
	    jpfPassword.setText("guest");
      }
      else
	  {
	    jtfUserName.setText("");
	    jpfPassword.setText("");
      }
  }
}

class LoginDialog_jbCancel_actionAdapter implements java.awt.event.ActionListener {
  LoginDialog adaptee;

  LoginDialog_jbCancel_actionAdapter(LoginDialog adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jbCancel_actionPerformed(e);
  }
}

class LoginDialog_jbOK_actionAdapter implements java.awt.event.ActionListener {
  LoginDialog adaptee;

  LoginDialog_jbOK_actionAdapter(LoginDialog adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jbOK_actionPerformed(e);
  }
}

class LoginDialog_jcbRememberLoginInformation_actionAdapter implements java.awt.event.ActionListener {
  LoginDialog adaptee;

  LoginDialog_jcbRememberLoginInformation_actionAdapter(LoginDialog adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jcbRememberLoginInformation_actionPerformed(e);
  }
}

class LoginDialog_jcbLoginAsGuest_actionAdapter implements java.awt.event.ActionListener {
  LoginDialog adaptee;

  LoginDialog_jcbLoginAsGuest_actionAdapter(LoginDialog adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jcbLoginAsGuest_actionPerformed(e);
  }
}