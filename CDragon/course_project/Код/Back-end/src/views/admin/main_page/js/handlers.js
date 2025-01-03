function applyFilters() {
  const searchTitle = document.getElementById("search-title").value;
  const statusFilter = document.getElementById("status-filter").value;
  const releaseDateStart = document.getElementById("release-date-start").value;
  const releaseDateEnd = document.getElementById("release-date-end").value;

  const categoryFilter = Array.from(
    document.getElementById("category-filter").selectedOptions
  ).map((option) => option.value);

  const keywordFilter = Array.from(
    document.getElementById("keyword-filter").selectedOptions
  ).map((option) => option.value);

  const tagFilter = Array.from(
    document.getElementById("tag-filter").selectedOptions
  ).map((option) => option.value);

  const filters = {
    searchTitle,
    statusFilter,
    releaseDateStart,
    releaseDateEnd,
    categoryFilter,
    keywordFilter,
    tagFilter,
  };

  fetch("/admin/main_page/filter_handler", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(filters),
  })
    .then((response) => {
      if (!response.ok) {
        alert("Сервер не дал положительный ответ об сортировке");
      }
      return response.text();
    })
    .then((htmlData) => {
      const spElement = document.querySelector("div.sp");
      if (spElement) {
        spElement.innerHTML = htmlData;
      } else {
        alert("Не отобразить полученные даные");
      }
    })
    .catch((error) => {
      alert("Ошибка при отправке данных:", error);
    });
}
