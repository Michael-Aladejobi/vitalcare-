// --------------------------------------------------------------------------------------------------------------------------------
//Sign Up | Register (START)
// --------------------------------------------------------------------------------------------------------------------------------

// Firebase auth DB
// Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.8.1/firebase-app.js";
import {
    getAuth,
    createUserWithEmailAndPassword,
} from "https://www.gstatic.com/firebasejs/9.8.1/firebase-auth.js";

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
const auth = getAuth(app);

// Event listener for creating a new account btn
document
    .getElementById("create-account-el")
    .addEventListener("click", function (event) {
        event.preventDefault();

        // Get inputs
        const email = document.getElementById("email-el").value;
        const password = document.getElementById("password-el").value;

        // Create user with email and password
        createUserWithEmailAndPassword(auth, email, password)
            .then((userCredential) => {
                // Signed up
                const user = userCredential.user;
                alert("Account Created Successfully!");
                window.location.href = "index.html";
            })
            .catch((error) => {
                const errorCode = error.code;
                const errorMessage = error.message;
                alert(errorMessage);
            });
    });
// --------------------------------------------------------------------------------------------------------------------------------
//Sign Up | Register (END)
// --------------------------------------------------------------------------------------------------------------------------------
