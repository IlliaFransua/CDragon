document.addEventListener("DOMContentLoaded", () => {
  fetch("/get_filter_data", {
    method: "GET",
    headers: {
      "Content-Type": "application/json",
    },
  })
    .then((response) => {
      if (!response.ok) {
        alert("Не удалось получить данные фильтров с серверва");
      }
      return response.json();
    })
    .then((data) => {
      const categorySelect = document.getElementById("category-filter");
      data.categories.forEach((category) => {
        const option = document.createElement("option");
        option.value = category.name;
        option.textContent = category.name;
        categorySelect.appendChild(option);
      });

      const keywordSelect = document.getElementById("keyword-filter");
      data.keywords.forEach((keyword) => {
        const option = document.createElement("option");
        option.value = keyword.keyword;
        option.textContent = keyword.keyword;
        keywordSelect.appendChild(option);
      });

      const tagSelect = document.getElementById("tag-filter");
      data.tags.forEach((tag) => {
        const option = document.createElement("option");
        option.value = tag.name;
        option.textContent = tag.name;
        tagSelect.appendChild(option);
      });
    })
    .catch((error) => {
      console.error("Ошибка при загрузке данных фильтров: ", error);
    });
});
