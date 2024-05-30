import { initializeApp } from "https://www.gstatic.com/firebasejs/9.8.1/firebase-app.js";
import {
    getAuth,
    onAuthStateChanged,
    signOut,
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
const user = auth.currentUser;

console.log(user);
onAuthStateChanged(auth, (user) => {
    if (user) {
        // User is signed in, see docs for a list of available properties
        // https://firebase.google.com/docs/reference/js/auth.user
        updateUserProfile(user);

        const uid = user.uid;
        return uid;

        // ...
    } else {
        // User is signed out, redirect  to the reggistration page
        // ...
        alert("Create Account & Login!");
        window.location.href = "signup.html";
    }
});

function updateUserProfile(user) {
    const userEmail = user.email;

    document.getElementById(
        "logged-in-as"
    ).innerHTML = ` LoggedIn as: <br><small> ${userEmail} </small>`;
}