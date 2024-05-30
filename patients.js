// Import Firebase modules
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.10.0/firebase-app.js";
import {
    getFirestore,
    collection,
    getDocs,
} from "https://www.gstatic.com/firebasejs/9.10.0/firebase-firestore.js";

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
const app = initializeApp(firebaseConfig);
const db = getFirestore(app);

// Fetch and display patient data
const displayPatients = async () => {
    const patientData = document.getElementById("patient-data");

    try {
        const querySnapshot = await getDocs(collection(db, "patients"));
        querySnapshot.forEach((doc) => {
            const patient = doc.data();
            const row = document.createElement("tr");
            row.innerHTML = `
                <td><a href="profile.html?id=${doc.id}">${doc.id}</a></td>
                <td>${patient.firstName} ${patient.lastName}</td>
                <td>${patient.email}</td>
                <td>${patient.address}</td>
                <td>${patient.gender}</td>
                <td>${patient.phoneNo}</td>
                <td>${patient.date}</td>
                <td><img src="${patient.imageUrl}" alt="Patient Image" width="50" height="50"></td>
            `;
            patientData.appendChild(row);
        });
    } catch (e) {
        console.error("Error fetching patient data: ", e);
    }
};

// invoke the display function
displayPatients();
