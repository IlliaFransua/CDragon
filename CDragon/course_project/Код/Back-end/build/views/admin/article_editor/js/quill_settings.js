const toolbarOptions = [
  ["bold", "italic", "underline", "strike"],
  ["blockquote", "code-block"],
  ["link", "image", "video"],

  [{ list: "ordered" }, { list: "bullet" }, { list: "check" }],
  [{ indent: "-1" }, { indent: "+1" }],

  [{ size: ["huge", "large", false, "small"] }],

  [{ color: [] }, { background: [] }],
  [{ font: [] }],
  [{ align: [] }],

  ["clean"],
];

var summaryEditor;
var contentEditor;
var seoDescriptionEditor;

document.addEventListener("DOMContentLoaded", function () {
  summaryEditor = new Quill("#summary-editor", {
    placeholder: "Текст...",
    theme: "bubble",
    modules: {
      toolbar: false,
    },
    formats: [],
  });

  contentEditor = new Quill("#content-editor", {
    placeholder: "Текст...",
    theme: "bubble",
    modules: {
      syntax: true,
      toolbar: toolbarOptions,
    },
  });

  seoDescriptionEditor = new Quill("#seo_description-editor", {
    placeholder: "Текст...",
    theme: "bubble",
    modules: {
      toolbar: false,
    },
    formats: [],
  });
});
