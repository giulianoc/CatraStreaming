package StreamingGUI;

import java.util.*;
import java.net.*;
import javax.swing.*;
import java.io.*;
import java.awt.Frame;
import java.util.StringTokenizer;
import XMLTree.*;

public class Refresh extends Functionality

{

  public Refresh (Frame fParentFrame)
  {
    super (fParentFrame);
  }

  public void realize (XMLTree xtXMLTree)
  {

        {
          JOptionPane.showMessageDialog (null,
            "Refresh functionality",
            "XMLTreeForMonitor", JOptionPane.PLAIN_MESSAGE);
        }
  }
}
