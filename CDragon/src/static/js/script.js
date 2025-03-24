document.getElementById("registerBtn").addEventListener("click", function() {
    const username = document.getElementById("username").value;
    const password = document.getElementById("password").value;

    // Формируем объект с данными
    const userData = {
        username: username,
        password: password
    };

    // Отправка данных на сервер
    fetch('/register', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(userData) // Преобразуем объект в строку JSON
    })
    .then(response => {
        if (response.ok) {
            console.log("Регистрация прошла успешно");
        } else {
            console.log("Ошибка регистрации");
        }
    })
    .catch(error => {
        console.error("Ошибка:", error);
    });;
});
