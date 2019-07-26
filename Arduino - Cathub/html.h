
String html_1 ="<style> body { background-color: #ECF1F1; min-height: 100%; font-family: monospace; letter-spacing: 2px; } table, th, td { border: 1px solid #ECF1F1; text-align: center; border-collapse: collapse; margin: auto; background-color: #F7F9F9; padding: 1%; margin-top: 50px; } table{ width: 70%; } .title { text-align: center; color: #0CA1A6; font-size: 1.5em; padding-top: 50px; } .bttn { padding-bottom: 1%; padding-top: 1%; margin-left: 35%; margin-right: 35%; margin-top: 10%; background-color: #00979D; text-align: center; color: #F7F9F9; font-size: 1.4em; } .bttn:hover { background-color: #008184; } a { text-decoration: none; }</style><body> <div class=\"title\">Food dispenser report</div> <table> <tr> <th>Food/no Food</th> <th>Cat detected</th> <th>Time to reach the food</th> </tr>";
String table_cells;
String html_2 ="</table> <a href=\"/BACKBUTTON\"><div class=\"bttn\">Disconect</div></a></body>";
String html;

void AddTableCell(bool food, String detected, String time){
  String cell="";
  if(food){
    cell="<tr><td>Food</td>";
  }else{
    cell="<tr><td>No Food</td>";
  }
  cell+="<td>"+detected+"</td>";
  cell+= "<td>"+ time + " s</td></tr>";

  table_cells+=cell;
  html = html_1 + table_cells + html_2;
}