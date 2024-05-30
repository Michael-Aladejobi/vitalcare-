// import { initializeApp } from "https://www.gstatic.com/firebasejs/9.10.0/firebase-app.js";
// import { getFirestore, doc, getDoc, collection, query, where, getDocs } from "https://www.gstatic.com/firebasejs/9.10.0/firebase-firestore.js";

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
// const app = initializeApp(firebaseConfig);
// const db = getFirestore(app);

// // Get patient ID from URL parameters
// const urlParams = new URLSearchParams(window.location.search);
// const patientId = urlParams.get('id');

// // Fetch and display patient information
// const displayPatientInfo = async () => {
//     const patientProfile = document.getElementById('patient-profile');

//     try {
//         const docRef = doc(db, "patients", patientId);
//         const docSnap = await getDoc(docRef);

//         if (docSnap.exists()) {
//             const patient = docSnap.data();
//             patientProfile.innerHTML = `
//                 <img src="${patient.imageUrl}" alt="Patient Image" width="100" height="100">
//                 <h3>${patient.firstName} ${patient.lastName}</h3>
//                 <p>Email: ${patient.email}</p>
//                 <p>Address: ${patient.address}</p>
//                 <p>Gender: ${patient.gender}</p>
//                 <p>Phone: ${patient.phoneNo}</p>
//                 <p>Date Registered: ${patient.date}</p>
//             `;
//         } else {
//             patientProfile.innerHTML = `<p>No patient data found for ID: ${patientId}</p>`;
//         }
//     } catch (e) {
//         console.error("Error fetching patient data: ", e);
//     }
// };

// // Fetch and display test results
// const displayTestResults = async () => {
//     const testResults = document.getElementById('test-data');

//     try {
//         const q = query(collection(db, "testResults"), where("patientId", "==", patientId));
//         const querySnapshot = await getDocs(q);

//         querySnapshot.forEach((doc) => {
//             const test = doc.data();
//             const row = document.createElement('tr');
//             row.innerHTML = `
//                 <td>${test.pulse}</td>
//                 <td>${test.temp}</td>
//                 <td>${test.height}</td>
//                 <td>${test.weight}</td>
//             `;
//             testResults.appendChild(row);
//         });
//     } catch (e) {
//         console.error("Error fetching test results: ", e);
//     }
// };

// // Run the display functions
// displayPatientInfo();
// displayTestResults();

///////////////////////////////////////////////
//BOUNDARY
/////////////////////////////////////////////

import { initializeApp } from "https://www.gstatic.com/firebasejs/9.10.0/firebase-app.js";
import {
    getFirestore,
    doc,
    getDoc,
    collection,
    query,
    where,
    getDocs,
    onSnapshot,
} from "https://www.gstatic.com/firebasejs/9.10.0/firebase-firestore.js";

// Your web app's Firebase configuration
const firebaseConfig = {
    apiKey: "AIzaSyBAgeUvIPwMirPSPywX9YZJZ79htSpKO5c",
    authDomain: "wrud-5b418.firebaseapp.com",
    projectId: "wrud-5b418",
    storageBucket: "wrud-5b418.appspot.com",
    messagingSenderId: "867302937144",
    appId: "1:867302937144:web:152fa283505a65facd4ccd",
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getFirestore(app);

// Get patient ID from URL parameters
const urlParams = new URLSearchParams(window.location.search);
const patientId = urlParams.get("id");

// Fetch and display patient information
const displayPatientInfo = async () => {
    const patientProfile = document.getElementById("patient-profile");

    try {
        const docRef = doc(db, "patients", patientId);
        const docSnap = await getDoc(docRef);

        if (docSnap.exists()) {
            const patient = docSnap.data();
            patientProfile.innerHTML = `
                <h3>${patient.firstName} ${patient.lastName}</h3>
                <p style="float:right; 
                border: 1px solid grey; 
                border-radius: 20px;
                padding: 5px 15x;">ID: ${patientId}</p>
                <p>Email: ${patient.email}</p>
                <p>Address: ${patient.address}</p>
                <p>Gender: ${patient.gender}</p>
                <p>Phone: ${patient.phoneNo}</p>
                <p>Date Registered: ${patient.date}</p>
            `;
        } else {
            patientProfile.innerHTML = `<p>No patient data found for ID: ${patientId}</p>`;
        }
    } catch (e) {
        console.error("Error fetching patient data: ", e);
    }
};

//Run a Test Now button
document.querySelector("#run-test-now").addEventListener("click", function () {
    alert("Processing...");
});

// // Fetch and display test results
// const displayTestResults = async () => {
//     const testResults = document.getElementById("patient-data");

//     try {
//         const q = query(
//             collection(db, "testResults"),
//             where("patientId", "==", patientId)
//         );
//         onSnapshot(q, (querySnapshot) => {
//             testResults.innerHTML = ""; // Clear previous results
//             querySnapshot.forEach((doc) => {
//                 const test = doc.data();
//                 const row = document.createElement("tr");
//                 row.innerHTML = `
//                     <td>${test.pulse}</td>
//                     <td>${test.temp}</td>
//                     <td>${test.height}</td>
//                     <td>${test.weight}</td>
//                 `;
//                 testResults.appendChild(row);
//             });
//         });
//     } catch (e) {
//         console.error("Error fetching test results: ", e);
//     }
// };

// Run the display functions
displayPatientInfo();
