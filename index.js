// Firebase auth DB
// Import the functions you need from the SDKs you need

import { initializeApp } from "https://www.gstatic.com/firebasejs/9.8.1/firebase-app.js";
import {
    getAuth,
    sendPasswordResetEmail,
    signInWithEmailAndPassword,
} from "https://www.gstatic.com/firebasejs/9.8.1/firebase-auth.js";

// My web app's Firebase configuration
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
const auth = getAuth(app);

document

    .getElementById("submit-btn")
    .addEventListener("click", function (event) {
        event.preventDefault();
        alert("Checking Details...\nPlease Wait!");
        // Get inputs
        const email = document.getElementById("email-el").value;
        const password = document.getElementById("password-el").value;

        signInWithEmailAndPassword(auth, email, password)
            .then((userCredential) => {
                // Signed in
                const user = userCredential.user;
                alert("Login Sucessful!");
                // ...
                window.location.href = "dashboard.html";
            })
            .catch((error) => {
                const errorCode = error.code;
                const errorMessage = error.message;
                alert(errorMessage);
            });
    });

const reset = document.getElementById("forget-password");
reset.addEventListener("click", function (event) {
    event.preventDefault();
    const email = document.getElementById("email-el").value;

    sendPasswordResetEmail(auth, email)
        .then(() => {
            // Password reset email sent!
            // ..

            alert("Email Sent! Check your inbox.");
        })
        .catch((error) => {
            const errorCode = error.code;
            const errorMessage = error.message;
            // ..
            alert(errorMessage);
        });
});
