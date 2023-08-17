const myApiKey = 'AIzaSyD8swBxHqSxipQH0081nh1TeAbaJq_nopE';
var stockApp = SpreadsheetApp.openById('1RqkSwwPLCqThSZnhgMXBxikelgPwbHokmvepFjmlrn4')
var stockSheet = stockApp.getSheetByName('stock-market')


function doGet(e) {
  if (e.parameter.read !== undefined) {
    var stockData = stockSheet.getDataRange().getValues();
    var stockJSON = JSON.stringify(stockData);
    return ContentService.createTextOutput(stockJSON).setMimeType(ContentService.MimeType.JSON);
  }
}