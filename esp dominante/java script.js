function handleData() {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', '/data', true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            const datos = JSON.parse(xhr.responseText);
            document.getElementById('humedad').innerText = datos.humedad;
            document.getElementById('temperatura').innerText = datos.temperatura;
            document.getElementById('PH').innerText = datos.PH;
            document.getElementById('Cloro').innerText = datos.Cloro;
        }
    };
    xhr.send();
}
document.addEventListener('DOMContentLoaded', () => {
    handleData();
    setInterval(handleData, 5000); // Actualiza los datos cada 5 segundos
});

async function handleData() {
    try {
        const response = await fetch('/data');
        if (response.ok) {
            const datos = await response.json();
            document.getElementById('humedad').innerText = datos.humedad;
            document.getElementById('temperatura').innerText = datos.temperatura;
            document.getElementById('PH').innerText = datos.PH;
            document.getElementById('Cloro').innerText = datos.Cloro;

            actualizarEstadoRele('rele1', datos.rele1);
            actualizarEstadoRele('rele2', datos.rele2);
            actualizarEstadoRele('rele3', datos.rele3);
            actualizarEstadoRele('rele4', datos.rele4);
            actualizarEstadoRele('rele5', datos.rele5);
            actualizarEstadoRele('rele6', datos.rele6);
            actualizarEstadoRele('rele7', datos.rele7);
            actualizarEstadoRele('rele8', datos.rele8);
            actualizarEstadoRele('rele9', datos.rele9);
            actualizarEstadoRele('rele10', datos.rele10);
            actualizarEstadoRele('rele11', datos.rele11);
            actualizarEstadoRele('rele12', datos.rele12);
            actualizarEstadoRele('rele13', datos.rele13);
            actualizarEstadoRele('rele14', datos.rele14);
        } else {
            console.error('Error en la solicitud:', response.status);
        }
    } catch (error) {
        console.error('Error en la solicitud:', error);
    }
}

function actualizarEstadoRele(releId, estado) {
    const statusElement = document.getElementById(`${releId}-status`);
    if (estado) {
        statusElement.classList.remove('red');
        statusElement.classList.add('green');
    } else {
        statusElement.classList.remove('green');
        statusElement.classList.add('red');
    }
}