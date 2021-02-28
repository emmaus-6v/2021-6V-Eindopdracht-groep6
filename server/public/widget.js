=============================VARIABELEN===============================
var achtergrondPlaatje;
var laatsteUpdateTimeStamp;
var sensorState1;
var f = createFont("fantasy");
var sensor1 = "niet doorbroken";
var sensor2 = "niet doorbroken";
var aantalKnikkers = 0;


==============================PRELOAD==================================
function preload() {
  achtergrondPlaatje = loadImage('images/janken.jpg');
}


======================DATABASE UPDATE CHECK=========================
function checkForDatabaseChanges() {
  // zet het serverrequest in elkaar
  var request = new XMLHttpRequest();
  request.open('GET', `/api/checkchanges/${laatsteUpdateTimeStamp}`, true)
  request.onload = function () {
    if (request.status >= 200 && request.status < 400) {
      if (this.response == "Update needed") {
        console.log("Server geeft aan dat de database een update heeft die widget nog niet heeft");

        // roep ander update functie(s) aan:
        getSensorState();
      }
      else {
        // je kunt de code hieronder aanzetten, maar krijgt dan wel iedere seconde een melding
        // console.log("Widget is up to date");
      }
    }
    else {
        console.log("bleh, server reageert niet zoals gehoopt");
        console.log(this.response);
      }
  }

  // verstuur het request
  request.send()
}


============================SENSOR SATUS===============================
/**
 * Vraagt senasorstatus op
 */
function getSensorState() {
  // zet het serverrequest in elkaar
  var request = new XMLHttpRequest()
  request.open('GET', '/api/sensorstate', true)
  request.onload = function () {
    var data = JSON.parse(this.response);
    if (request.status >= 200 && request.status < 400) {
      console.log(`Status van sensor 1 = ${data.sensorstate} `);
      sensor1 = "???";
      var newTimeStamp = new Date(data.lasttimestamp).getTime()+1;

      // update indien nodig de timestamp
      if (laatsteUpdateTimeStamp < newTimeStamp) {
        laatsteUpdateTimeStamp = newTimeStamp;
      }
      
    }
    else {
        console.log("bleh, server reageert niet zoals gehoopt");
        console.log(this.response);
      }
  }

  // verstuur het request
  request.send()
}


function Sensor1(){
    if (sensor1 === "doorbroken"){
        fill(180, 40, 40);        
        text("SENSOR 1:  doorbroken", 15, 140);
    }
    else{
        fill(40, 130, 40);
        text("SENSOR 1:  niet doorbroken", 15, 140);
    }
}

function Sensor2() {
    if (sensor2 === "doorbroken"){
        fill(180, 40, 40);
        text("SENSOR 2:  doorbroken", 15, 165);
    }
    else{
        fill(40, 130, 40);
        text("SENSOR 2:  niet doorbroken", 15, 165);
    }
}


====================================AANTAL KNIKKERS======================================


========================SETUP========================
function setup() {
  // Maak het canvas van je widget
  createCanvas(480, 200);

  // zet timeStamp op lang geleden zodat we alle recente info binnenkrijgen
  laatsteUpdateTimeStamp = new Date().setTime(0);

  // we vragen elke 2 seconde of er iets is veranderd
  setInterval(checkForDatabaseChanges, 2000);
}


=========================DRAW==========================
function draw() {
  image(achtergrondPlaatje, 0, 0, 480, 200);

  textFont(f, 25);
  fill(0, 0, 0);
  text("KNIKKERBAAN GROEP 6", 15, 15, 500, 500);
    
  textSize(15);
  text("AANTAL LANGSGEKOMEN KNIKKERS:", 15, 65, 500, 500);
  text(aantalknikkers, 15, 90, 500, 500);
  Sensor1();    
  Sensor2();
    
  stroke(0, 0, 0);
  fill(110, 146, 255);
  ellipse(mouseX, mouseY, 15, 15);
}

