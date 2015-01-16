
package StreamingGUI;

import javax.swing.table.AbstractTableModel;
import java.util.Vector;
import java.awt.event.MouseEvent;
import java.util.StringTokenizer;



public class GlobalConnectedUsersMonitoringTableModel extends AbstractTableModel

{

  private int                         _iRowsNumber;
  private int                         _iColumnsNumber;
  private String                      _psColumnsNames [];
  private String                      _psValues [];


  public GlobalConnectedUsersMonitoringTableModel(int iRowsNumber)
  {
    int                         iColumnIndex;

    _iRowsNumber          = iRowsNumber;
    _iColumnsNumber       = 8;

    _psColumnsNames = new String [_iColumnsNumber];

    _psColumnsNames [0]       = "Site";
    _psColumnsNames [1]       = "Server";
    _psColumnsNames [2]       = "IP Address";
    _psColumnsNames [3]       = "Bit rate";
    _psColumnsNames [4]       = "Bytes sent";
    _psColumnsNames [5]       = "% packet lost";
    _psColumnsNames [6]       = "Time connected";
    _psColumnsNames [7]       = "Filename (# times streamed)";

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
