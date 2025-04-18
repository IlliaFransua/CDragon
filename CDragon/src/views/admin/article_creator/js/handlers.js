function saveArticle() {
  const title = document.getElementById("title").value;
  const summary = JSON.stringify(summaryEditor.getContents());
  const content = JSON.stringify(contentEditor.getContents());
  const seoDescription = JSON.stringify(seoDescriptionEditor.getContents());
  const status = document.getElementById("status").value;
  const publishDate = document.getElementById("datetime").value;
  const slug = document.getElementById("slug").value;
  const seoTitle = document.getElementById("seo_title").value;
  const keywords = document
    .getElementById("keywords")
    .value.trim()
    .split(" ")
    .filter(Boolean);
  const categories = Array.from(
    document.getElementById("category-filter").selectedOptions
  ).map((option) => option.innerText);
  const tags = Array.from(
    document.getElementById("tag-filter").selectedOptions
  ).map((option) => option.innerText);
  const previewImage = document.getElementById("preview_image").files[0];

  let previewImageBase64 = "";

  if (previewImage) {
    const reader = new FileReader();
    reader.onloadend = () => {
      previewImageBase64 = reader.result.split(",")[1];

      const data = {
        title,
        summary,
        content,
        seoDescription,
        status,
        publishDate,
        slug,
        seoTitle,
        keywords,
        categories,
        tags,
        previewImage: previewImageBase64,
      };

      sendData(data);
    };
    reader.readAsDataURL(previewImage);
  } else {
    const data = {
      title,
      summary,
      content,
      seoDescription,
      status,
      publishDate,
      slug,
      seoTitle,
      keywords,
      categories,
      tags,
      previewImage: previewImageBase64,
    };

    sendData(data);
  }
}

function sendData(data) {
  fetch("/admin/article_creator_handler", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(data),
  })
    .then((response) => {
      if (response.ok) {
        alert("Статья успешно сохранена");
      } else {
        alert("Ошибка при сохранении статьи на сервере");
      }
    })
    .catch((error) => {
      alert("Произошла ошибка при отправке данных: " + error);
    });
}

function exitButton() {
  window.location.href = "/admin/main_page";
}

document.getElementById("save_button").addEventListener("click", saveArticle);
document.getElementById("exit_button").addEventListener("click", exitButton);

document.addEventListener("DOMContentLoaded", function () {
  const apiUrl = "/admin/get_json_of_all_categories";

  fetch(apiUrl)
    .then((response) => {
      if (!response.ok) {
        alert("Не удалось получить данные о категориях");
      }
      return response.json();
    })
    .then((data) => {
      const categorySelect = document.getElementById("category-filter");

      categorySelect.innerHTML = "";

      if (data.categories && Array.isArray(data.categories)) {
        data.categories.forEach((category) => {
          const option = document.createElement("option");
          option.value = category.id;
          option.textContent = category.name;
          categorySelect.appendChild(option);
        });
      } else {
        alert("С сервера получен неверный формат данных о категориях");
      }
    })
    .catch((error) => {
      console.error("Произошла ошибка:", error);
    });
});

document.addEventListener("DOMContentLoaded", function () {
  const apiUrl = "/admin/get_json_of_all_tags";

  fetch(apiUrl)
    .then((response) => {
      if (!response.ok) {
        alert("Не удалось получить данные о категориях");
      }
      return response.json();
    })
    .then((data) => {
      const tagSelect = document.getElementById("tag-filter");

      tagSelect.innerHTML = "";

      if (data.tags && Array.isArray(data.tags)) {
        data.tags.forEach((tag) => {
          const option = document.createElement("option");
          option.value = tag.id;
          option.textContent = tag.name;
          tagSelect.appendChild(option);
        });
      } else {
        alert("С сервера получен неверный формат данных о тегах");
      }
    })
    .catch((error) => {
      console.error("Произошла ошибка:", error);
    });
});
