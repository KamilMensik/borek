# frozen_string_literal: true

# Player1 paddle
class PaddleL < Borek::Scriptable
  def on_update(delta)
    axis = Borek::Input.axis
    transform.position.y += axis.y * 200 * delta
  end
end
