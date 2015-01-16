
package StreamingGUI;

import javax.swing.table.AbstractTableModel;
import java.util.Vector;
import java.awt.event.MouseEvent;
import java.util.StringTokenizer;



public class SiteConnectedUsersMonitoringTableModel extends AbstractTableModel

{

  private int                         _iRowsNumber;
  private int                         _iColumnsNumber;
  private String                      _psColumnsNames [];
  private String                      _psValues [];


  public SiteConnectedUsersMonitoringTableModel(int iRowsNumber)
  {
    int                         iColumnIndex;

    _iRowsNumber          = iRowsNumber;
    _iColumnsNumber       = 7;

    _psColumnsNames = new String [_iColumnsNumber];

    _psColumnsNames [0]       = "Server";
    _psColumnsNames [1]       = "IP Address";
    _psColumnsNames [2]       = "Bit rate";
    _psColumnsNames [3]       = "Bytes sent";
    _psColumnsNames [4]       = "% packet lost";
    _psColumnsNames [5]       = "Time connected";
    _psColumnsNames [6]       = "Filename (# times streamed)";

    _psValues = new String [_iRowsNumber * _iColumnsNumber];

    for (iColumnIndex = 0; iColumnIndex < _iColumnsNumber; iColumnIndex++)
      setValue (0, iColumnIndex, "-");
  }

  public int setValue (int iRowIndex, int iColumnIndex, String sValue)
  {
    if (_psValues [iRowIndex * _iColumnsNumber + iColumnIndex] == null)
      _psValues [iRowIndex * _iColumnsNumber + iColumnIndex]        =
        new String ();

    _psValues [iRowIndex * _iColumnsNumber + iColumnIndex]      = sValue;

    return 0;
  }

  public void setColumnNameAt (int iColumnIndex, String pColumnName)
  {
    _psColumnsNames [iColumnIndex]    			= pColumnName;
  }

  public Class getColumnClass(int col)
  {
    return getValueAt(0,col).getClass();
  }

  public int getRowCount()
  {
    return _iRowsNumber;
  }

  public int getColumnCount()
  {

    return _iColumnsNumber;
  }

  public String getColumnName (int column)
  {

    return _psColumnsNames [column];
  }

  public void setValueAt (Object oValue, int iRowIndex, int iColumnIndex)
  {
    _psValues [iRowIndex * _iColumnsNumber + iColumnIndex]        =
      (String) oValue;
  }

  public Object getValueAt (int row, int column)
  {
    return _psValues [row * _iColumnsNumber + column];
  }

  public boolean isCellEditable (int iRowIndex, int iColumnIndex)
  {
    return false;
  }
}
