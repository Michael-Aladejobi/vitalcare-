document.getElementById("cancel-btn").addEventListener("click", function (e) {
    e.preventDefault();
    alert("Canceled Successfully!");
    window.location.href = "add_New_Patient.html";
});

// Import Firebase modules
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.10.0/firebase-app.js";
import {
    getFirestore,
    collection,
    addDoc,
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

// Reference to the form element
const patientForm = document.getElementById("patient-info");

// Add a submit event listener to the form
patientForm.addEventListener("submit", async (e) => {
    e.preventDefault();

    // Get the form data
    const firstName = document.getElementById("firstname-el").value;
    const lastName = document.getElementById("lastname-el").value;
    const email = document.getElementById("email-el").value;
    const address = document.getElementById("address-el").value;
    const gender = document.getElementById("gender-el").value;
    const phoneNo = document.getElementById("phone-no-el").value;
    const date = document.getElementById("date-el").value;
    const image = document.getElementById("image-el").value;

    try {
        // Add a new document with the form data
        const docRef = await addDoc(collection(db, "patients"), {
            firstName: firstName,
            lastName: lastName,
            email: email,
            address: address,
            gender: gender,
            phoneNo: phoneNo,
            date: date,
            Image: image,
        });

        alert("Processing...");
        console.log("Document written with ID: ", docRef.id);
        alert("Patient information stored successfully!");

        // Clear the form
        patientForm.reset();
        window.location.href = "patients.html";
    } catch (e) {
        console.error("Error adding document: ", e);
        alert("Error storing patient information: " + e);
    }
});
