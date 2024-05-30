// // .....................
// // SNGLE U-SENSOR READINGS
// // .....................
// // // Your web app's Firebase configuration
// // const firebaseConfig = {
// //     apiKey: "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c",
// //     authDomain: "wrud-5b418.firebaseapp.com",
// //     databaseURL: "https://wrud-5b418-default-rtdb.firebaseio.com",
// //     projectId: "wrud-5b418",
// //     storageBucket: "wrud-5b418.appspot.com",
// //     messagingSenderId: "867302937144",
// //     appId: "1:867302937144:web:152fa283505a65facd4ccd",
// // };

// // // Initialize Firebase
// // firebase.initializeApp(firebaseConfig);

// // // Set database variable
// // var database = firebase.database();

// // // Function to display all data
// // function displayAll() {
// //     var dataTableBody = document.getElementById("data_table_body");

// //     // Listen for changes in the database
// //     var users_ref = database.ref("UsersData");
// //     users_ref.on("child_added", function (snapshot) {
// //         var patientID = snapshot.key;
// //         var user = snapshot.val();
// //         var readings = user.readings;

// //         // Display all readings for this patient
// //         for (let readingID in readings) {
// //             let reading = readings[readingID];
// //             var row = dataTableBody.insertRow();
// //             row.insertCell(0).appendChild(document.createTextNode(patientID));
// //             row.insertCell(1).appendChild(document.createTextNode(readingID));
// //             row.insertCell(2).appendChild(
// //                 document.createTextNode(reading.distance)
// //             );
// //             row.insertCell(3).appendChild(
// //                 document.createTextNode(reading.timestamp)
// //             );
// //         }
// //     });
// // }

// // // Example function to save data with your own patient IDs
// // function saveReading(patientID, readingID, distance, timestamp) {
// //     var readingRef = database.ref(
// //         "UsersData/" + patientID + "/readings/" + readingID
// //     );
// //     readingRef.set({
// //         distance: distance,
// //         timestamp: timestamp,
// //     });
// // }

// // // Call displayAll to set up the listener and display data
// // displayAll();

// // .....................
// // TWO U-SENSOR READINGS
// // .....................

// // Your web app's Firebase configuration
// const firebaseConfig = {
//     apiKey: "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c",
//     authDomain: "wrud-5b418.firebaseapp.com",
//     databaseURL: "https://wrud-5b418-default-rtdb.firebaseio.com",
//     projectId: "wrud-5b418",
//     storageBucket: "wrud-5b418.appspot.com",
//     messagingSenderId: "867302937144",
//     appId: "1:867302937144:web:152fa283505a65facd4ccd",
// };

// // Initialize Firebase
// firebase.initializeApp(firebaseConfig);

// // Set database variable
// var database = firebase.database();

// // Function to display all data
// function displayAll() {
//     var dataTableBody = document.getElementById("data_table_body");

//     // Listen for changes in the database
//     var users_ref = database.ref("UsersData");
//     users_ref.on("child_added", function (snapshot) {
//         var patientID = snapshot.key;
//         var user = snapshot.val();
//         var readings = user.readings;

//         // Display all readings for this patient
//         for (let readingID in readings) {
//             let reading = readings[readingID];
//             var row = dataTableBody.insertRow();
//             row.insertCell(0).appendChild(document.createTextNode(patientID));
//             row.insertCell(1).appendChild(document.createTextNode(readingID));
//             row.insertCell(2).appendChild(
//                 document.createTextNode(reading.distance1) // Change to distance1
//             );
//             row.insertCell(3).appendChild(
//                 document.createTextNode(reading.distance2) // Add distance2
//             );
//             row.insertCell(4).appendChild(
//                 document.createTextNode(reading.timestamp)
//             );
//         }
//     });
// }

// // Example function to save data with my own patient IDs
// function saveReading(patientID, readingID, distance1, distance2, timestamp) {
//     var readingRef = database.ref(
//         "UsersData/" + patientID + "/readings/" + readingID
//     );
//     readingRef.set({
//         distance1: distance1, // Update - include distance1
//         distance2: distance2, // Add distance2
//         timestamp: timestamp,
//     });
// }

// // Call displayAll to set up the listener and display data
// displayAll();

//...............
// Firebase's real-time database event listener(firebase Ajax)
//................

// Your web app's Firebase configuration
const firebaseConfig = {
    apiKey: "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c",
    authDomain: "wrud-5b418.firebaseapp.com",
    databaseURL: "https://wrud-5b418-default-rtdb.firebaseio.com",
    projectId: "wrud-5b418",
    storageBucket: "wrud-5b418.appspot.com",
    messagingSenderId: "867302937144",
    appId: "1:867302937144:web:152fa283505a65facd4ccd",
};

// Initialize Firebase
firebase.initializeApp(firebaseConfig);

// Set database variable
var database = firebase.database();

// Get patient ID from URL parameters
const urlParams = new URLSearchParams(window.location.search);
const patientId = urlParams.get("id");

// Function to display data for a specific patient
function displayPatientData(patientId) {
    var dataTableBody = document.getElementById("data_table_body");

    // Reference to the specific patient's data
    var patientRef = database.ref("UsersData/" + patientId + "/readings");

    // Use "on" method to listen for real-time updates
    patientRef.on("child_added", function (snapshot) {
        var readingID = snapshot.key;
        var reading = snapshot.val();
        addTableRow(dataTableBody, patientId, readingID, reading);
    });

    // Listen for updates in existing data
    patientRef.on("child_changed", function (snapshot) {
        var readingID = snapshot.key;
        var reading = snapshot.val();

        // Clear existing rows for this patient
        var rows = dataTableBody.rows;
        for (let i = rows.length - 1; i >= 0; i--) {
            if (
                rows[i].cells[0].innerText === patientId &&
                rows[i].cells[1].innerText === readingID
            ) {
                dataTableBody.deleteRow(i);
            }
        }

        // Display updated reading for this patient
        addTableRow(dataTableBody, patientId, readingID, reading);
    });

    // Listen for removed data
    patientRef.on("child_removed", function (snapshot) {
        var readingID = snapshot.key;

        // Remove rows corresponding to the removed reading
        var rows = dataTableBody.rows;
        for (let i = rows.length - 1; i >= 0; i--) {
            if (
                rows[i].cells[0].innerText === patientId &&
                rows[i].cells[1].innerText === readingID
            ) {
                dataTableBody.deleteRow(i);
            }
        }
    });
}

// Function to add a row to the table
function addTableRow(dataTableBody, patientID, readingID, reading) {
    var row = dataTableBody.insertRow();
    row.insertCell(0).appendChild(document.createTextNode(patientID));
    row.insertCell(1).appendChild(document.createTextNode(readingID));
    row.insertCell(2).appendChild(document.createTextNode(reading.temperature));
    row.insertCell(3).appendChild(document.createTextNode(reading.pulse));
    row.insertCell(4).appendChild(document.createTextNode(reading.height));
    row.insertCell(5).appendChild(document.createTextNode(reading.weight));
    row.insertCell(6).appendChild(document.createTextNode(reading.timestamp));
}

// Example function to save data with your own patient IDs
function saveReading(
    patientID,
    readingID,
    temperature,
    pulse,
    height,
    weight,
    timestamp
) {
    var readingRef = database.ref(
        "UsersData/" + patientID + "/readings/" + readingID
    );
    readingRef.set({
        temperature: temperature,
        pulse: pulse,
        height: height,
        weight: weight,
        timestamp: timestamp,
    });
}

// Call displayPatientData with the specified patient ID
if (patientId) {
    displayPatientData(patientId);
} else {
    console.error("No patient ID specified in the URL.");
}
